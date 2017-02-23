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

namespace {
}

QEOrmModel::QEOrmModel( const QMetaObject* metaObj)
	: QEAnnotationModel( metaObj) 
{
  	d_ptr = new QEOrmModelPrivate( *this, metaObj);
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
{ return d_ptr->table(); }

vector<QEOrmColumnDef> QEOrmModel::primaryKey() const noexcept
{ return d_ptr->primaryKey(); }

QEOrmColumnDef QEOrmModel::findColumnByProperty(const QString &property) const noexcept
{
	return d_ptr->findColumnDefIf( 
			[&property]( const QEOrmColumnDef& colDef)
				{ return colDef.propertyName() == property;});
}

QEOrmColumnDef QEOrmModel::findColumnByName(const QString &columnName) const noexcept
{
	return d_ptr->findColumnDefIf( 
			[&columnName]( const QEOrmColumnDef& colDef)
				{ return colDef.dbColumnName() == columnName;});
}

#if 0
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
#endif

QEOrmColumnDef QEOrmModel::findAutoIncrementColumn() const noexcept
{
	return d_ptr->findColumnDefIf( 
			[]( const QEOrmColumnDef& colDef)
				{ return colDef.isDbAutoIncrement();});
}

vector< QEOrmColumnDef > QEOrmModel::columns() const noexcept
{ return d_ptr->columns(); }


#if 0
QString QEOrmModel::getReferenceName( QString columnName) const
{
	QEOrmColumnDef findDef = columnByName( columnName);
	if( findDef.isValid())
	{
		uint id = 0;
		QString columnNameBase = QString( "%1%2").arg( model.table()).arg( columnName);
		findDef = columnByName( columnNameBase);
		while( findDef.isValid())
		{
			columnNameBase = QString( "%1%2%3").arg( model.table()).arg( columnName).arg(++id);
			findDef = columnByName( columnNameBase);
		}
		columnName = columnNameBase;
	}
	return columnName;
}
#endif

void QEOrmModel::addRefToOne( const QEOrmModel &reference)
{
	QEOrmForeignDef fk( *this, reference, reference.primaryKey());
	d_ptr->addReferencesToOne( fk);
}

vector<QEOrmForeignDef> QEOrmModel::referencesToOne() const noexcept
{ return d_ptr->referencesToOne(); }
