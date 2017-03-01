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
#pragma once
#include "QEOrmForeignDef.hpp"
#include <QEAnnotation/QEAnnotation.hpp>
#include <functional>

QE_BEGIN_NAMESPACE

class QEOrm;
class QEOrmModelPrivate;
class QEOrmModel;
using QEOrmModelShd = std::shared_ptr<QEOrmModel>;

/// @brief This model parse the Orm annotations
class QEOrmModel : public QEAnnotationModel
{
	friend class QEOrm;
	public:
		using QEOrmColumnDefList = std::vector<QEOrmColumnDefShd>;
		using QEOrmForeignDefList = std::vector<QEOrmForeignDefShd>;
		using FindColDefPredicate = std::function<bool(const QEOrmColumnDefShd&)>;

		struct findByPropertyName { QString name; };
		struct findByColumnName { QString name; };
		struct findByAutoIncrement {};

		/// @brief Create and parse annotations from @p meta.
		explicit QEOrmModel( const QMetaObject* meta);

		// DB 	
		/// @return Database table name.
		const QString& table() const noexcept;
		/// @return Model column definitions.
		const QEOrmColumnDefList& columnDefs() const noexcept;
		/// @return Model primary key definition.
		const QEOrmColumnDefList& primaryKeyDef() const noexcept;
		/// @return A list of references of type Many to one.
		const QEOrmForeignDefList& referencesManyToOneDefs() const noexcept;
	
		/// @brief Add reference Many (this model) to one @p reference
		void addReferenceManyToOne( const QByteArray& propertyName, 
				const QEOrmModelShd& reference);

		// Find utils	
		/// @brief It finds column definitions by property name.
		QEOrmColumnDefShd findColumnDef( const findByPropertyName& pn) const noexcept;
		/// @brief It finds column definitions by table column name.
		QEOrmColumnDefShd findColumnDef( const findByColumnName& cn) const noexcept;
		/// @brief It finds column definition if auto_increment is enabled. 
		QEOrmColumnDefShd findColumnDef( const findByAutoIncrement& ) const noexcept;
		/// @brief It finds the column definition that returns true on @p
		/// predicate.
		QEOrmColumnDefShd findColumnDef( FindColDefPredicate&& predicate) const noexcept;

		/// @brief It returns the foreign key definition for specific model. 
		const QEOrmForeignDefShd findForeignTo( const QEOrmModelShd& model) const noexcept;	

	protected:
		QEOrmModel( const QEOrmModel& ) = delete;
		QEOrmModel& operator=( const QEOrmModel& ) = delete;
		
	private:
		/// @brief It parses the annotation from @p meta.
		void parseAnnotations( const QMetaObject* meta);
		
	private:
		QString m_table;								///< Database table name.
		QEOrmColumnDefList m_columnDefs;			///< Model column definitions.
		QEOrmColumnDefList m_primaryKeyDef;		///< Model primary key definition.

		QEOrmForeignDefList m_referencesManyToOneDefs;	///< Many to one defs.

		QEOrmModelPrivate *d_ptr;
		Q_DECLARE_PRIVATE(QEOrmModel)
};

QE_END_NAMESPACE
