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
 */

#include "QEOrmColumnDef.hpp"
#include "QEOrmColumnDefPrivate.hpp"
#include <QEOrmModel.hpp>
#include <utility>

using namespace std;

QEOrmColumnDef::QEOrmColumnDef()
	: d_ptr( new QEOrmColumnDefPrivate)
{}

QEOrmColumnDef::QEOrmColumnDef( const QEOrmColumnDef &other) noexcept
	: d_ptr( other.d_ptr)
{}

QEOrmColumnDef::QEOrmColumnDef( const QString &property, const int type, const QEAnnotationModel *model)
	: d_ptr( new QEOrmColumnDefPrivate( property, type, model))
{}

bool QEOrmColumnDef::isValid() const noexcept
{ return ! d_ptr->propertyName.isEmpty(); }

QString QEOrmColumnDef::propertyName() const noexcept
{ return d_ptr->propertyName; }

int QEOrmColumnDef::propertyType() const noexcept
{ return d_ptr->propertyType;}



QString QEOrmColumnDef::dbColumnName() const noexcept
{ return d_ptr->dbColumnName; }

QVariant QEOrmColumnDef::dbDefaultValue() const noexcept
{ return d_ptr->dbDefaultValue; }

uint QEOrmColumnDef::dbMaxLength() const noexcept
{ return d_ptr->dbMaxLength; }

bool QEOrmColumnDef::isDbAutoIncrement() const noexcept
{ return d_ptr->isDbAutoIncrement; }

bool QEOrmColumnDef::isDbNullable() const noexcept
{ return d_ptr->isDbNullable; }

bool QEOrmColumnDef::isDbUnique() const noexcept
{ return d_ptr->isDbUnique; }
