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
#include <qe/common/Exception.hpp>
#include <qe/entity/ModelRepository.hpp>
#include <qe/entity/Model.hpp>
#include <qe/entity/EntityDef.hpp>

#include <QVariantList>
#include <iterator>

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

		return target->sqlHelper().exitsObject( model, pkValues);
	}

	void update( ObjectContext& context, const Model &model, 
			const QObject *source, SerializedItem* const target)
	{
		target->sqlHelper().update( context, model, source);
	}

	void insert( ObjectContext& context, const Model &model, QObject *source, 
			qe::orm::SerializedItem* const target)
	{
		target->sqlHelper().insert( context, model, source);
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

	target->sqlHelper().createTableIfNotExist( *model);
	
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

