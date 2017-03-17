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
 *
 * GNU Lesser General Public License Usage
 * Alternatively, this file may be used under the terms of the GNU Lesser
 * General Public License version 3 as published by the Free
 * Software Foundation and appearing in the file LICENSE.LGPLv3 
 * included in the packaging of this file. Please review the
 * following information to ensure the GNU Lesser General Public License
 * requirements will be met: https://www.gnu.org/licenses/lgpl.html and
 * http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
 *
 * $QE_END_LICENSE$
 */

#pragma once
#include <QECommon/QEGlobal.hpp>
#include <QLoggingCategory>
#include <QString>
#include <QByteArray>
#include <QVariant>
#include <memory>

Q_DECLARE_LOGGING_CATEGORY( QeOrmColumnDef)
QE_BEGIN_NAMESPACE 

class QEAnnotationModel;
class QEOrmColumnDefPrivate;

/// @brief This class defines the relation between a property and a database
/// column
class QEOrmColumnDef
{
	Q_GADGET
	public:
		enum MappingType { 
			NoMappingType, 
			OneToOne, OneToMany, 
			ManyToOne, ManyToMany };
		Q_ENUM( MappingType );

		enum MappingFetch { Direct, Lazy };
		Q_ENUM( MappingFetch );
		
	public:
		/// @brief It creates a column definition 
		/// @param property Property name.
		/// @param type Property type.
		/// @param model It will use this model to extract annotations.
		QEOrmColumnDef( 
			const QByteArray &property, 
			const int type, 
			const QEAnnotationModel &model);

		/// @brief  It creates a column definition 
		/// @param propertyName Property name.
		/// @param propertyType Property type.
		/// @param dbColumnName Table column name.
		/// @param dbMaxLength Table column maximun length. 
		QEOrmColumnDef(
				const QByteArray &propertyName,
				const int propertyType,
				const QString& dbColumnNmae,
				const uint dbMaxLength);

	private:
		Q_DISABLE_COPY( QEOrmColumnDef)

		/// @brief It decodes properties for this column from @p model.
		void decodeProperties(const QEAnnotationModel &model);

		/// @brief It decodes mapping information.
		void decodeMapping(const QEAnnotationModel &model);

	public:
		QByteArray propertyName;	///< Property name.
		int propertyType;				///< Property type.
		
		QString dbColumnName;		///< DB column name
		QVariant dbDefaultValue;	///< Default value.
	
		MappingType mappingType 	= MappingType::NoMappingType;
		MappingFetch mappingFetch 	= MappingFetch::Direct;
		const QMetaObject* mappingEntity = nullptr;
		
		uint dbMaxLength 			= 0;
		bool isDbAutoIncrement 	= false;
		bool isDbNullable 		= true;
		bool isPartOfPrimaryKey	= false;

		QEOrmColumnDefPrivate* d_ptr;
		Q_DECLARE_PRIVATE(QEOrmColumnDef)
};
using QEOrmColumnDefShd = std::shared_ptr<QEOrmColumnDef>;

QE_END_NAMESPACE
