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
 */

#include "QEOrmModel.hpp"
#include <QEAnnotation/QEAnnotationModel.hpp>
#include <QMetaProperty>
#include <utility>

using namespace std;
namespace {
	inline QString ANN_CLASS_ID() { return QLatin1Literal( "class");}
	inline QString ANN_TABLE_KEY() { return QLatin1Literal( "@QE.ORM.TABLE");}
	inline QString ANN_TEMPORARY_TABLE_KEY() { return QLatin1Literal( "@QE.ORM.TEMPORARY_TABLE");}
	
	inline QString ANN_EXPORT_PARENT_KEY() { return  QLatin1Literal( "@QE.ORM.EXPORT_PARENT");}
	inline QString ANN_PRIMARY_KEY() { return QLatin1Literal( "@QE.ORM.PRIMARY_KEY");}
	inline QString ANN_INDEX() { return QLatin1Literal( "@QE.ORM.INDEX");}
	
	inline QString ANN_ENABLE() { return QLatin1Literal( "@QE.ORM.ENABLE");}
}

QEOrmModel::QEOrmModel( const QMetaObject* metaObj)
	: QEAnnotationModel( metaObj)
{ 
	parseAnnotations( metaObj);
}

QEOrmModel::QEOrmModel(const QEOrmModel &model) noexcept
	: QEAnnotationModel( model), d_ptr( model.d_ptr)
{ }

QEOrmModel &QEOrmModel::operator=(const QEOrmModel & model) noexcept
{
	QEAnnotationModel::operator=( model);
	d_ptr = model.d_ptr;
	return *this;
}

void QEOrmModel::parseAnnotations( const QMetaObject* metaObj )
{
	// Table
	d_ptr->table = annotation( ANN_CLASS_ID(), ANN_TABLE_KEY())
		.value( QString( metaObj->className())).toString();
	// Columns			
	const bool exportParents = annotation( ANN_CLASS_ID(), ANN_EXPORT_PARENT_KEY())
		.value( false).toBool();
	const int begin = (exportParents) ? 0 : metaObj->propertyOffset();
	for( int i = begin; i < metaObj->propertyCount(); ++i)
	{
		QMetaProperty property = metaObj->property(i);
		const QString propertyName = property.name();
		const bool isEnable = annotation( propertyName, ANN_ENABLE())
			.value( true).toBool();
		if( isEnable )
		{
			d_ptr->columnsByProperty.insert( 
				make_pair( propertyName,
						   QEOrmColumnDef( propertyName, property.type(), this)));
		}
	}
	
	// Primary keys
	d_ptr->primaryKey = annotation( ANN_CLASS_ID(), ANN_PRIMARY_KEY())
		.value( QString()).toString()
		.split( ',', QString::SkipEmptyParts);
		
	// Indexes
	/// @todo Indexes
}

QString QEOrmModel::table() const noexcept
{ return d_ptr->table; }

QStringList QEOrmModel::primaryKey() const noexcept
{ return d_ptr->primaryKey; }

QEOrmColumnDef QEOrmModel::columnByProperty(const QString &property) const noexcept
{
	const auto itr = d_ptr->columnsByProperty.find( property);
	if( itr != end(d_ptr->columnsByProperty))
		return itr->second;
	
	return QEOrmColumnDef();
}

QEOrmColumnDef QEOrmModel::columnByName(const QString &columnName) const noexcept
{
	const auto itr = find_if( 
		begin(d_ptr->columnsByProperty), 
		end(d_ptr->columnsByProperty),
		[&columnName]( const QEOrmModelPrivate::ColumnMapByProperty::value_type& item)
			{ return columnName == item.second.columnName();});
	
	if( itr != end( d_ptr->columnsByProperty))
		return itr->second;
	
	return QEOrmColumnDef();
}

QStringList QEOrmModel::columnNames() const
{
	QStringList columnNames;
	transform( 
		begin(d_ptr->columnsByProperty), 
		end(d_ptr->columnsByProperty),
		std::back_inserter(columnNames),
		[]( const QEOrmModelPrivate::ColumnMapByProperty::value_type& item)
		{ return item.second.columnName(); });

	return columnNames;
}

