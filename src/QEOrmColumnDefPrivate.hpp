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
#pragma once

#include <QECommon/QEGlobal.hpp>
#include <QObject>
#include <QSharedData>
#include <QString>
#include <QVariant>

QE_BEGIN_NAMESPACE 

class QMetaObject;
class QEAnnotationModel;
class QEOrmColumnDefPrivate : public QSharedData
{
	Q_GADGET
	public:
		enum MappingType { NoMappingType, OneToOne, OneToMany, ManyToOne, ManyToMany };
		Q_ENUM( MappingType );
		enum MappingFetch { Direct, Lazy };
		Q_ENUM( MappingFetch );
	
	public:
		QEOrmColumnDefPrivate();
		QEOrmColumnDefPrivate( const QByteArray &property, const int type, const QEAnnotationModel &model);
		QEOrmColumnDefPrivate( const QEOrmColumnDefPrivate& other);
		
	private:
		void decodeMapping( const QEAnnotationModel &model);

	public:
		QByteArray propertyName;
		int propertyType;
		
		QString dbColumnName;
		QVariant dbDefaultValue;
	
		MappingType mappingType = MappingType::NoMappingType;
		MappingFetch mappingFetch = MappingFetch::Direct;
		const QMetaObject* mappingEntity = nullptr;
		
		uint dbMaxLength 			= 0;
		bool isDbAutoIncrement 	= false;
		bool isDbNullable 		= true;
		bool isDbUnique 			= false;
		bool isPartOfPrimaryKey	= false;
};

QE_END_NAMESPACE
