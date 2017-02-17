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
#include <QEOrmModel.hpp>
#include <utility>

using namespace std;
namespace {
	inline QString ANN_AUTO_INCREMENT() { return QLatin1Literal( "@QE.ORM.AUTO_INCREMENT");}
	inline QString ANN_DB_COLUMN() { return QLatin1Literal( "@QE.ORM.DB_COLUMN");}
	inline QString ANN_MAX_LENGTH() { return QLatin1Literal( "@QE.ORM.MAX_LENGTH");}
	inline QString ANN_NULL() { return QLatin1Literal( "@QE.ORM.NULL");}
	inline QString ANN_UNIQUE() { return QLatin1Literal( "@QE.ORM.UNIQUE");}
	inline QString ANN_DEFAULT() { return QLatin1Literal( "@QE.ORM.DEFAULT");}
}

QEOrmColumnDef::QEOrmColumnDef() = default;
QEOrmColumnDef::QEOrmColumnDef( const QEOrmColumnDef &other) = default;

QEOrmColumnDef::QEOrmColumnDef( 
	const QString property, const int type, const QEOrmModel *model)
	: m_propertyName(property), m_propertyType( type)
{
	m_columnName = model->annotation( property, ANN_DB_COLUMN())
		.value( property).toString();
	m_isNull = model->annotation( property, ANN_NULL())
		.value( false).toBool();
	m_isAutoIncrement = model->annotation( property, ANN_AUTO_INCREMENT())
		.value( false).toBool();
	m_isUnique = model->annotation( property, ANN_UNIQUE())
		.value( false).toBool();
	m_maxLength = model->annotation( property, ANN_MAX_LENGTH())
		.value( 0).toUInt();
		
	if( type == QMetaType::Char 
		|| type == QMetaType::QChar
		|| type == QMetaType::SChar
		|| type == QMetaType::UChar)
		m_maxLength = 1;
}


bool QEOrmColumnDef::isValid() const noexcept
{ return !m_columnName.isEmpty();}

QString QEOrmColumnDef::columnName() const noexcept
{ return m_columnName; }

QString QEOrmColumnDef::propertyName() const noexcept
{ return m_propertyName; }

uint QEOrmColumnDef::maxLength() const noexcept
{ return m_maxLength;}

bool QEOrmColumnDef::isAutoIncrement() const noexcept
{ return m_isAutoIncrement;}

bool QEOrmColumnDef::isNull() const noexcept
{ return m_isNull;}

bool QEOrmColumnDef::isUnique() const noexcept
{ return m_isUnique;}

int QEOrmColumnDef::propertyType() const noexcept
{ return m_propertyType;}
