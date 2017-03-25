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
#include "serialization/SerializedItem.hpp"
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

	/// @brief It checks if @p source exits into database.
	bool exists(const Model &model, const QObject *source, 
			SerializedItem* const target)
	{
		QVariantList pkValues;
		for( const auto& colDef: model.primaryKeyDef())
		{
			const QVariant value = source->property( colDef->propertyName());
			if( colDef->isAutoIncrement() && value.toInt() == 0)
				pkValues << QVariant();
			else
				pkValues << value;
		}

		// Execute sql
		const sql::Executor& sqlExec = target->executor();
		const QString stmt = GeneratorRepository::instance()
			.generator( sqlExec.dbmsType())->existsStatement( model);

		QSqlQuery query = sqlExec.execute( stmt, pkValues, 
			QStringLiteral( "QE Orm cannot check existence of object in database."));

		return query.next();
	}

	void update( ObjectContext& context, const Model &model, 
			const QObject *source, SerializedItem* const target)
	{
		const sql::Executor& sqlExec = target->executor();
		const QString stmt = GeneratorRepository::instance()
			.generator( sqlExec.dbmsType())->updateStatement( model);

		sqlExec.execute( context, model, stmt, source, 
			QStringLiteral( "QE Orm cannot update an object"));
	}

	void insert( ObjectContext& context, const Model &model, QObject *source, 
			qe::orm::SerializedItem* const target)
	{
		// 1. Insert
		const sql::Executor& sqlExec = target->executor();
		const QString stmt = GeneratorRepository::instance()
			.generator( sqlExec.dbmsType())->insertStatement( model);

		QSqlQuery ds = sqlExec.execute( context, model, stmt, source, 
			QStringLiteral( "QE Orm cannot insert an object"));

		// 2. Get autoincrement value if it exists.
		const QVariant insertId = ds.lastInsertId();
		if( ! insertId.isNull())
		{
			const auto eDef = model.findEntityDef( Model::findByAutoIncrement{});
			if( eDef)
				source->setProperty( eDef->propertyName(), insertId);
		}
	}
}

// SaveHelper
// ============================================================================

void SaveHelper::save( ObjectContext& context, const ModelShd& model, 
	QObject *const source, SerializedItem* const target) const
{
	// Check recursive relations
	if( find( begin(context), end(context), source) != end(context))
		Exception::makeAndThrow(
			QStringLiteral( "SQL Save helper does NOT support recursive relations"));	

	if( exists( *model, source, target))
		update( context, *model, source, target);
	else
		insert( context, *model, source, target);

	saveOneToMany(context, *model, source, target);
}

void SaveHelper::saveOneToMany( ObjectContext& context, const Model &model, 
		QObject *source, SerializedItem* const target) const
{
	ScopedStackedObjectContext _( source, context);
	
	for( const auto& colDef : model.entityDefs())
	{
		if( colDef->mappingType() == EntityDef::MappingType::OneToMany)
		{
			const QByteArray& propertyName = colDef->propertyName();
			const QVariant propertyValue = source->property( propertyName);
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
					ModelShd refModel = ModelRepository::instance().model( refItem->metaObject());
					save( context, refModel, refItem, target);
				}
			}
		}
	}
}


QStringList SaveHelper::createTables( const ModelShd model, 
	SerializedItem* const target) const
{
	QStringList tables;
	tables << model->name();

	// Create table
	const sql::Executor& sqlExec = target->executor();
	const QString stmt = GeneratorRepository::instance()
			.generator( sqlExec.dbmsType())->createTableIfNotExistsStatement( *model);

	sqlExec.execute( stmt, QVariantList{}, 
			QStringLiteral("QE Orm cannot create the table ")
			% model->name());

	// Find relations.
	for( const auto& colDef: model->entityDefs())
	{
		if( colDef->mappingType() == EntityDef::MappingType::OneToMany)
		{
			auto mapModel = ModelRepository::instance().model( colDef->mappingEntity());
			const auto refTables = createTables( mapModel, target);
			copy( begin(refTables), end(refTables), back_inserter(tables));
		}
	}

	return tables;
}

