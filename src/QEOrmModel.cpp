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
#include "QEOrmModelPrivate.hpp"
#include <QEAnnotation/QEAnnotationModel.hpp>
#include <QMetaProperty>
#include <utility>

using namespace std;

QEOrmModel::QEOrmModel( const QMetaObject* metaObj)
	: QEAnnotationModel( metaObj), d_ptr( new QEOrmModelPrivate)
{ 
	d_ptr->parseAnnotations( this, metaObj);
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


QString QEOrmModel::table() const noexcept
{ return d_ptr->table; }

vector<QEOrmColumnDef> QEOrmModel::primaryKey() const noexcept
{ return d_ptr->primaryKey; }

vector< QEOrmColumnDef > QEOrmModel::noPrimaryKey() const noexcept
{ return d_ptr->noPrimaryKey; }

QEOrmColumnDef QEOrmModel::columnByProperty(const QString &property) const noexcept
{
	const auto itr = d_ptr->columnsByProperty.find( property);
	if( itr != end(d_ptr->columnsByProperty))
		return itr->second;
	
	return QEOrmColumnDef();
}

QEOrmColumnDef QEOrmModel::columnByName(const QString &columnName) const noexcept
{
	const auto itr = d_ptr->columnsByName.find( columnName);
	if( itr != end( d_ptr->columnsByName))
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
			[]( const QEOrmModelPrivate::ColumnDefBy::value_type& item)
			{ return item.second.dbColumnName(); });

	return columnNames;
}

QString QEOrmModel::autoIncrementColumnName() const
{
	QString columnName;

	const auto itr = find_if( 
		begin(d_ptr->columnsByProperty), 
		end(d_ptr->columnsByProperty),
		[]( const QEOrmModelPrivate::ColumnDefBy::value_type& item)
			{ return item.second.isDbAutoIncrement();});
	
	if( itr != end( d_ptr->columnsByProperty))
		columnName = itr->second.dbColumnName();

	return columnName;
}


