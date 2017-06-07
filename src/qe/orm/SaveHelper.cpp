/*
 * Copyright (C) 2017 Francisco Miguel García Rodríguez
 * Contact: http://www.dmious.com/qe/licensing/
 *
 * This file is part of the QE Common module of the QE Toolkit.
 *
 * $QE_BEGIN_LICENSE$
 * Commercial License Usage
 * Licensees holding valid commercial QE licenses may use this file in
 * accordance with the commercial license agreement provided with the
 * Software or, alternatively, in accordance with the terms contained in
 * a written agreement between you and The Dmious Company. For licensing terms
 * and conditions see http://www.dmious.com/qe/terms-conditions. For further
 * information use the contact form at http://www.dmious.com/contact-us.
 * 
 * GNU Lesser General Public License Usage
 * Alternatively, this file may be used under the terms of the GNU Lesser
 * General Public License version 3 as published by the Free Software
 * Foundation and appearing in the file LICENSE.LGPL3 included in the
 * packaging of this file. Please review the following information to
 * ensure the GNU Lesser General Public License version 3 requirements
 * will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
 *
 * $QE_END_LICENSE$
 */
#include "SaveHelper.hpp"
#include "S11nContext.hpp"
#include "sql/GeneratorRepository.hpp"
#include "sql/generator/AbstractGenerator.hpp"
#include <qe/common/Exception.hpp>
#include <qe/entity/ModelRepository.hpp>
#include <qe/entity/Model.hpp>
#include <qe/entity/EntityDef.hpp>

#include <QVariantList>
#include <QStringBuilder>
#include <iterator>

using namespace qe::orm::sql;
using namespace qe::orm;
using namespace qe::entity;
using namespace qe::common;
using namespace std;

struct TableStatement {
	QString tableName;
	QString sqlStatement;
};
using TableStatementList = std::vector<TableStatement>;

namespace {

	const QObject* getPropertySource(
		const EntityDef& eDef,
		S11nContext* const context,
		const QObject* defaultSource)
	{
		if( eDef.mappedType() != EntityDef::MappedType::NoMappedType)
			defaultSource = context->top();

		return defaultSource;
	}

	/// @brief It checks if @p source exits into database.
	/// It uses the primary key from @p model.
	bool exists(
			const Model &model, 
			const QObject *source, 
			S11nContext* const context)
	{
		QVariantList pkValues;
		const auto & modelPk = model.primaryKeyDef();
		for( const auto& colDef: modelPk)
		{
			const QObject* propertySource = getPropertySource( colDef, context, source);
			if( propertySource)
			{
				const QVariant value = propertySource->property( colDef.propertyName());
				if( colDef.isAutoIncrement() && value.toInt() == 0)
					pkValues << QVariant();
				else
					pkValues << value;
			}
		}

		// Execute sql
		const auto stmtMaker = context->statementMaker();
		const QString stmt = stmtMaker->existsStatement( model);

		QSqlQuery query = context->execute( 
				stmt, 
				pkValues, 
				QStringLiteral( "QE Orm cannot check existence of object in database."));

		return query.next();
	}

	/// @brief It updates the database record using data from @p source.
	void update( 
			const Model &model, 
			const QObject *source, 
			S11nContext* const context)
	{
		const QString stmt = context->statementMaker()
			->updateStatement( model);

		context->execute( 
			model, 
			stmt, 
			source, 
			QStringLiteral( "QE Orm cannot update an object"));
	}

	/// @brief It inserts into database from @p source.
	void insert( 
			const Model &model, 
			QObject *source, 
			S11nContext* const context)
	{
		// 1. Insert
		const QString stmt = context->statementMaker()
			->insertStatement( model);

		QSqlQuery ds = context->execute( 
				model, 
				stmt, 
				source, 
				QStringLiteral( "QE Orm cannot insert an object"));

		// 2. Get autoincrement value if it exists.
		const QVariant insertId = ds.lastInsertId();
		if( ! insertId.isNull())
		{
			const auto eDef = model.findEntityDef(
				FindEntityDefByAutoIncrement{});
			if( eDef)
				source->setProperty( eDef->propertyName(), insertId);
		}
	}

#if 0
	QString createOneToManySimpleType(
		const ModelShd fkModel,
		S11nContext* const context,
		const EntityDef& eDef)
	{
		const QString table = fkModel->name() % "_" % name;
		const auto fk = fkModel->primaryKeyDef();

		const QString stmt = context->statementMaker()
			->createOneToManySimpleType(
				table,
				fk,
				eDef);

		context->execute(
			stmt,
			QVariantList(),
			QStringLiteral( "QE Orm cannot create 'one to many' for a simple type"));

		return table;
	}
#endif
}

// SaveHelper
// ============================================================================

SaveHelper::~SaveHelper()
{}

void SaveHelper::save( 
		const Model& model,
		QObject *const source, 
		S11nContext* const context) const
{
	// Check recursive relations
	if( context->isObjectInContext( source))
		Exception::makeAndThrow(
			QStringLiteral( "SQL Save helper does NOT support recursive relations"));	

	if( exists( model, source, context))
		update( model, source, context);
	else
		insert( model, source, context);

	saveOneToMany( model, source, context);
}


void SaveHelper::saveOneToManyUserType(
	const QByteArray& propertyName,
	const QVariant& propertyValue,
	S11nContext* const context) const
{
	if( ! propertyValue.canConvert<QVariantList>())
		Exception::makeAndThrow(
			QStringLiteral("QE Orm can only use QVariantList for mapping property %1")
			.arg( propertyName.constData()));

	QVariantList values = propertyValue.toList();
	for( QVariant& value : values)
	{
		QObject *refItem = value.value<QObject*>();
		if( refItem )
		{
			Model refModel = ModelRepository::instance().model( refItem->metaObject());
			save( refModel, refItem, context);
		}
	}
}

void SaveHelper::saveOneToManyStrinList(
	const QByteArray& propertyName,
	const QVariant& propertyValue,
	const Model& refModel,
	S11nContext* const context ) const
{
	uint index = 0;
	const QStringList values = propertyValue.toStringList();

	for( const QString& value: values)
	{
		QObject adapter;
		adapter.setProperty( "idx", index++);
		adapter.setProperty( propertyName, value);
		save( refModel, &adapter, context);
	}
}

void SaveHelper::saveOneToMany( 
		const Model &model, 
		QObject *source, 
		S11nContext* const context) const
{
	ScopedS11Context _( source, context);
	
	for( const auto& colDef : model.entityDefs())
	{
		if( colDef.mappedType() == EntityDef::MappedType::OneToMany)
		{
			const QByteArray& propertyName = colDef.propertyName();
			const QVariant propertyValue = source->property( propertyName);
			const int type = colDef.propertyType();

			switch( type)
			{
				case QMetaType::Type::QVariantList:
					saveOneToManyUserType( propertyName, propertyValue, context);
					break;
				case QMetaType::Type::QStringList:
					{
						optional<Model> colDefModel = colDef.mappedModel();
						if( colDefModel)
							saveOneToManyStrinList( propertyName, propertyValue, *colDefModel, context);
						break;
					}
				default:
					Exception::makeAndThrow(
						QString( "Unsupported 'One to Many' relation for type %1 on '%2'")
							.arg( type).arg( colDef.entityName()));
			}
		}
	}
}

QStringList SaveHelper::createTables( 
	const Model& model,
	S11nContext* const context) const
{
	QStringList tables;
	tables << model.name();

	// Create table
	const QString stmt = context->statementMaker()
		->createTableIfNotExistsStatement( model);

	context->execute( 
			stmt, 
			QVariantList{}, 
			QStringLiteral("QE Orm cannot create the table ")
				% model.name());

	// Find relations.
	for( const auto& colDef: model.entityDefs())
	{
		const auto mappingType = colDef.mappedType();
		if( mappingType == EntityDef::MappedType::OneToMany)
		{
			const auto mappedModel = colDef.mappedModel();
			if( mappedModel)
			{
				const auto refTables = createTables( *mappedModel, context);
				copy( begin(refTables), end(refTables), back_inserter(tables));
			}
			else
			{
				Exception::makeAndThrow(
					QString( "Entity definition (%1) has not model")
						.arg( colDef.entityName()));
#if 0
				// Array of simple types.
				const auto refTable = createOneToManySimpleType( context);
				tables.push_back( refTable);
#endif
			}
		}
	}

	return tables;
}

