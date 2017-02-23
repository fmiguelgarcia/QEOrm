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
#include "QEOrmModel.hpp"
#include "QEOrmModelPrivate.hpp"
#include <QEAnnotation/QEAnnotationModel.hpp>
#include <QMetaProperty>
#include <utility>

using namespace std;

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

QEOrmColumnDef QEOrmModel::findAutoIncrementColumn() const noexcept
{
	return d_ptr->findColumnDefIf( 
			[]( const QEOrmColumnDef& colDef)
				{ return colDef.isDbAutoIncrement();});
}

vector< QEOrmColumnDef > QEOrmModel::columns() const noexcept
{ return d_ptr->columns(); }

void QEOrmModel::addRefToOne( const QByteArray& propertyName, const QEOrmModel &reference)
{
	QEOrmForeignDef fk( *this, propertyName, reference, reference.primaryKey());
	d_ptr->addReferencesToOne( fk);
}

vector<QEOrmForeignDef> QEOrmModel::referencesToOne() const noexcept
{ return d_ptr->referencesToOne(); }
