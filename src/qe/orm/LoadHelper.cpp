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
#include "serialization/SerializedItem.hpp"
#include <qe/entity/Model.hpp>
#include <qe/entity/ModelRepository.hpp>
#include <qe/entity/EntityDef.hpp>
#include <qe/common/Exception.hpp>

#include <QStringBuilder>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlField>

using namespace qe::common;
using namespace qe::entity;
using namespace qe::orm;

void LoadHelper::load( ObjectContext& context, const ModelShd& model, 
	const SerializedItem *const source, QObject *const target) const
{
	const QVariantList & pk = source->primaryKey();
	QSqlQuery ds = source->sqlHelper().loadUsingPrimaryKey( *model, pk);

	loadObjectFromRecord( *model, ds.record(), target);
	loadOneToMany( context, model, source, target);
}

void LoadHelper::loadObjectFromRecord( const Model& model, 
	const QSqlRecord& record, QObject *const target) const
{
	for( int i = 0; i < record.count(); ++i)
	{
		const QSqlField field = record.field( i);
		const auto colDef = model.findEntityDef( 
				Model::findByEntityName{ field.name()});
	
		if( colDef )
			target->setProperty( colDef->propertyName().constData(), 
					field.value());
	}
}

void LoadHelper::loadOneToMany( ObjectContext& context, const ModelShd& model,
	const SerializedItem *const source, QObject* const target) const
{
	ScopedStackedObjectContext _( target, context);
	
	for( const auto& colDef : model->entityDefs())
	{
		if( colDef->mappingType() == EntityDef::MappingType::OneToMany)
		{
			ModelShd manyModel = ModelRepository::instance().model( colDef->mappingEntity());
			auto fkDef = manyModel->findRelationTo( model);
			if( fkDef )
			{
				QVariantList wrapperList = loadObjectsUsingForeignKey(
					context, *manyModel, 
					*fkDef, source, 
					colDef->mappingEntity(), target);

				const QByteArray& propertyName = colDef->propertyName();
				target->setProperty( propertyName, wrapperList);
			}
		}
	}
}
			
QVariantList LoadHelper::loadObjectsUsingForeignKey( 
	ObjectContext& context, const Model& refModel,
	const RelationDef& fkDef, const SerializedItem* const source,
	const QMetaObject* refMetaObjectEntity, QObject *const target) const
{
	QVariantList list;
	ModelShd model = ModelRepository::instance().model( refMetaObjectEntity);

	// Cursor over many objects
	QSqlQuery ds = source->sqlHelper().loadUsingForeignKey( context, refModel, fkDef, 
		target);

	while( ds.next())
	{
		// Create object.
		QObject *refObj = refMetaObjectEntity->newInstance( Q_ARG( QObject*, target));
		if( !refObj)
			Exception::makeAndThrow(
				QStringLiteral( "QE Orm load helper cannot create an instance of class ")
				% refMetaObjectEntity->className());

		const QSqlRecord record = ds.record();
		loadObjectFromRecord( *model, record, refObj);
		list.push_back( QVariant::fromValue(refObj));
	}

	return list;
}

