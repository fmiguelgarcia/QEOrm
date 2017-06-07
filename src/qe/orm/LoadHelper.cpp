/*
 * Copyright (C) 2017 francisco miguel garcia rodriguez  
 * Contact: http://www.dmious.com/qe/licensing/
 *
 * This file is part of the QE Common module of the QE Toolkit.
 *
 * $QE_BEGIN_LICENSE:LGPL21$
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
 * General Public License version 3 as published by the Free
 * Software Foundation and appearing in the file LICENSE.LGPLv3 
 * included in the packaging of this file. Please review the
 * following information to ensure the GNU Lesser General Public License
 * requirements will be met: https://www.gnu.org/licenses/lgpl.html and
 * http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
 *
 * $QE_END_LICENSE$
 */
#include "LoadHelper.hpp"
#include "sql/GeneratorRepository.hpp"
#include "sql/generator/AbstractGenerator.hpp"
#include "S11nContext.hpp"

#include <qe/entity/Model.hpp>
#include <qe/entity/ModelRepository.hpp>
#include <qe/entity/EntityDef.hpp>
#include <qe/entity/RelationDef.hpp>
#include <qe/common/Exception.hpp>

#include <QStringBuilder>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlField>
#include <QSqlError>

using namespace qe::common;
using namespace qe::entity;
using namespace qe::orm;
using namespace std;

namespace {

	QSqlQuery execSelectUsingForeignKey (
		const Model& refModel,
		const RelationDef& relationDef,
		const S11nContext* const context,
		QObject* const target)
	{
		// Cursor over many objects
		const auto stmtMaker = context->statementMaker();
		const QString stmt = stmtMaker->selectionUsingForeignKey(
			refModel, relationDef);

		QSqlQuery ds = context->execute(
			refModel, stmt, target,
			QStringLiteral( "QE Orm cannot fetch object from a relation."));

		return ds;
	}

}

LoadHelper::~LoadHelper()
{}

void LoadHelper::load( 
	const Model& model,
	const S11nContext *const context, 
	QObject *const target) const
{
	const QVariantList & pk = context->primaryKey();
	const auto stmtMaker = context->statementMaker(); 
	const QString stmt = stmtMaker->selectionUsingPrimaryKey( model);

	QSqlQuery ds = context->execute( 
			stmt, 
			pk,  
			QStringLiteral( "QE Orm cannot fetch an object."));

	const bool resultAvailable = ds.next();
	if( resultAvailable)
	{
		loadObjectFromRecord( model, ds.record(), target);
		loadOneToMany( model, context, target);
	}
	else if( ds.lastError().type() != QSqlError::NoError)
	{
		QStringList pkStrValues;
		transform( begin(pk), end(pk), back_inserter(pkStrValues), 
			[]( const QVariant& var){ return var.toString();});

		Exception::makeAndThrow(
			QString( "QE Orm cannot fetch any row on table '%1' "
				"using the following values as primary key {%2}. "
				"SQL error: %3")
				.arg( model.name()).arg( pkStrValues.join( ","))
				.arg( ds.lastError().text()));
	}
}

void LoadHelper::loadObjectFromRecord( const Model& model, 
	const QSqlRecord& record, QObject *const target) const
{
	for( int i = 0; i < record.count(); ++i)
	{
		const QSqlField field = record.field( i);
		const auto colDef = model.findEntityDef( 
				FindEntityDefByEntityName{ field.name()});
	
		if( colDef )
			target->setProperty( colDef->propertyName().constData(), 
					field.value());
	}
}

void LoadHelper::loadOneToManyUserType(
	const Model& refModel,
	const RelationDef& relationDef,
	const S11nContext* const context,
	QObject* const target,
	const QByteArray& targetProperty ) const
{
	QVariantList wrapperList = loadObjectsUsingForeignKey(
		refModel, relationDef, context, target);

	target->setProperty( targetProperty, wrapperList);
}

void LoadHelper::loadOneToManyStringList(
	const Model& refModel,
	const RelationDef& relationDef,
	const S11nContext* const context,
	QObject* const target,
	const QByteArray& targetProperty) const
{
	QSqlQuery ds = execSelectUsingForeignKey (
		refModel, relationDef, context, target);

	QStringList values;
	QVariant value;
	int columnIdx = -1;
	while( ds.next())
	{
		if( Q_UNLIKELY(columnIdx == -1))
			columnIdx = ds.record().indexOf( targetProperty);

		if( Q_LIKELY(columnIdx != -1))
			values << ds.value( columnIdx).toString();
	}

	target->setProperty( targetProperty, values);
}

void LoadHelper::loadOneToMany( 
	const Model& model,
	const S11nContext *const context, 
	QObject* const target) const
{
	ScopedS11Context _( target, context);
	
	for( const auto& eDef : model.entityDefs())
	{
		if( eDef.mappedType() == EntityDef::MappedType::OneToMany)
		{
			optional<Model> manyModel = eDef.mappedModel();
			if( !manyModel)
				Exception::makeAndThrow(
					QString( "Mapped Model '%1:%2' can NOT be found")
						.arg( model.name(), eDef.entityName()));

			auto fkDef = manyModel->findRelationTo( model);
			if( !fkDef)
				Exception::makeAndThrow(
					QString( "Relation definition in '%1' model can NOT be found")
						.arg( manyModel->name()));

			const QByteArray& targetProperty = eDef.propertyName();
			const int type = eDef.propertyType();
			switch( type)
			{
				case QMetaType::Type::QVariantList:
					loadOneToManyUserType( *manyModel, *fkDef, context,  target, targetProperty);
					break;
				case QMetaType::Type::QStringList:
					loadOneToManyStringList( *manyModel, *fkDef, context, target, targetProperty);
					break;
				default:
					Exception::makeAndThrow(
						QString( "Unsupported 'One to Many' relation for type %1 on '%2'")
						.arg( type).arg( eDef.entityName()));
			}
		}
	}
}

QVariantList LoadHelper::loadObjectsUsingForeignKey( 
	const Model& refModel,
	const RelationDef& fkDef, 
	const S11nContext* const context,
	QObject *const target) const
{
	QVariantList list;
	const QMetaObject* refMetaObject = refModel.metaObject();
	QSqlQuery ds = execSelectUsingForeignKey (
		refModel, fkDef, context, target);

	while( ds.next())
	{
		// Create object.
		QObject *refObj = refMetaObject->newInstance( Q_ARG( QObject*, target));
		if( !refObj)
			Exception::makeAndThrow(
				QStringLiteral( "QE Orm load helper cannot create an instance of class ")
				% refMetaObject->className());

		const QSqlRecord record = ds.record();
		loadObjectFromRecord( refModel, record, refObj);
		list.push_back( QVariant::fromValue(refObj));
	}

	return list;
}

