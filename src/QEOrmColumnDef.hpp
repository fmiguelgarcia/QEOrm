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
#include <QEOrmColumnDefPrivate.hpp>
#include <QSharedDataPointer>
#include <QECommon/QEGlobal.hpp>
#include <QMetaType>
#include <QString>
#include <QVariant>

QE_BEGIN_NAMESPACE 

class QEOrmModel;
class QEAnnotationModel;
class QEOrmColumnDef
{
	friend class QEOrmModel;
	public:
		using MappingType = typename QEOrmColumnDefPrivate::MappingType;
		using MappingFetch = typename QEOrmColumnDefPrivate::MappingFetch;
	
	public:
		QEOrmColumnDef();
		QEOrmColumnDef( 
			const QByteArray &property, 
			const int type, 
			const QEAnnotationModel &model);
		QEOrmColumnDef( const QEOrmColumnDef &other) noexcept;

		bool isValid() const noexcept;

		// Property
		QByteArray propertyName() const noexcept;
		int propertyType() const noexcept;
	
		// DB	
		QString dbColumnName() const noexcept;
		void setDbColumnName( const QString& name);
		QVariant dbDefaultValue() const noexcept;
		uint dbMaxLength() const noexcept;
		bool isDbAutoIncrement() const noexcept;
		void setDbAutoIncrement( const bool v) noexcept;
		bool isDbNullable() const noexcept;
		bool isDbUnique() const noexcept;
		void setDbUnique( const bool v) noexcept;
		bool isPartOfPrimaryKey() const noexcept;
		void setPartOfPrimaryKey( const bool v) noexcept;
	
		//	
		MappingType mappingType() const noexcept;
		MappingFetch mappingFetch() const noexcept;
		const QMetaObject* mappingEntity() const noexcept;

	private:
		QSharedDataPointer<QEOrmColumnDefPrivate> d_ptr;
};

QE_END_NAMESPACE
