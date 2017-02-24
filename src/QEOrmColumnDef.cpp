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
#include <QDebug>
#include <QMetaEnum>
#include <utility>

QE_USE_NAMESPACE 
using namespace std;
namespace {
	inline QString ANN_AUTO_INCREMENT() 
	{ return QLatin1Literal( "@QEOrm.isAutoIncrement");}
	inline QString ANN_DB_COLUMN() 
	{ return QLatin1Literal( "@QEOrm.dbColumn");}
	inline QString ANN_MAX_LENGTH() 
	{ return QLatin1Literal( "@QEOrm.maxLength");}
	inline QString ANN_NULL() 
	{ return QLatin1Literal( "@QEOrm.isNull");}
	inline QString ANN_DEFAULT() 
	{ return QLatin1Literal( "@QEOrm.default");}
	
	inline QString ANN_MAPPING_TYPE() 
	{ return QLatin1Literal( "@QEOrm.mapping.type");}
	inline QString ANN_MAPPING_ENTITY() 
	{ return QLatin1Literal( "@QEOrm.mapping.entity");}
}

QEOrmColumnDef::QEOrmColumnDef(
				const QByteArray &propertyName,
				const int propertyType,
				const QString& dbColumnName,
				const uint dbMaxLength)
	: propertyName( propertyName), propertyType( propertyType),
	dbColumnName( dbColumnName), dbMaxLength( dbMaxLength)
{}

QEOrmColumnDef::QEOrmColumnDef( const QByteArray &property, const int type,
	  	const QEAnnotationModel &model)
	: propertyName(property), propertyType( type)
{
	decodeProperties( model);
	
	try {
		decodeMapping( model);
	} catch ( std::runtime_error &error){
		qWarning() << error.what();
		mappingType = MappingType::NoMappingType;
	}

	if( type == QMetaType::Char 
		|| type == QMetaType::QChar
		|| type == QMetaType::SChar
		|| type == QMetaType::UChar)
		dbMaxLength = 1;
}

void QEOrmColumnDef::decodeProperties(const QEAnnotationModel &model)
{
	dbColumnName = model.annotation( propertyName, ANN_DB_COLUMN())
		.value( propertyName).toString();
	isDbNullable = model.annotation( propertyName, ANN_NULL())
		.value( false).toBool();
	isDbAutoIncrement = model.annotation( propertyName, ANN_AUTO_INCREMENT())
		.value( false).toBool();
	dbMaxLength = model.annotation( propertyName, ANN_MAX_LENGTH())
        .value( 0).toUInt();
}

void QEOrmColumnDef::decodeMapping(const QEAnnotationModel &model)
{
	bool enumOk;
	QMetaEnum meMappingType = QMetaEnum::fromType< QEOrmColumnDef::MappingType>();
	const QString mappingTypeStr = model.annotation( propertyName, ANN_MAPPING_TYPE())
        .value( QLatin1Literal("NoMappingType")).toString();
	mappingType = static_cast<MappingType>( meMappingType.keyToValue( mappingTypeStr.toLocal8Bit().constData(), &enumOk)); 
	if( !enumOk)
		throw runtime_error( QString("QE Orm cannot decode mapping type '%1' on property '%2'").arg( mappingTypeStr).arg( QString( propertyName)).toStdString());
	
	if( mappingType != MappingType::NoMappingType)
	{
        const QString mappingEntityName = model.annotation( propertyName, ANN_MAPPING_ENTITY()).value().toString();
		if( mappingEntityName.isEmpty())
			throw runtime_error( QString("QE Orm requires no empty mapping entity on property '%1'").arg( QString(propertyName)).toStdString());
		
		// Check metadata is avaible.
		const QString classPointer = mappingEntityName + QChar('*');
		const int typeId = QMetaType::type( classPointer.toLocal8Bit());
		if( typeId == QMetaType::UnknownType)
			throw runtime_error( QString("QE Orm cannot map entity of type '%1'").arg( mappingEntityName).toStdString());
		
		mappingEntity = QMetaType::metaObjectForType( typeId);
	}
}


