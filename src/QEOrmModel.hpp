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
#pragma once
#include "QEOrmForeignDef.hpp"
#include <QEAnnotation/QEAnnotation.hpp>
#include <functional>

QE_BEGIN_NAMESPACE

class QEOrm;
class QEOrmModelPrivate;
class QEOrmModel;
using QEOrmModelShd = std::shared_ptr<QEOrmModel>;

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

		explicit QEOrmModel( const QMetaObject* meta);

		// DB 	
		const QString& table() const noexcept;
		const QEOrmColumnDefList& columnDefs() const noexcept;
		const QEOrmColumnDefList& primaryKeyDef() const noexcept;
		const QEOrmForeignDefList& referencesManyToOneDefs() const noexcept;
	
		/// @brief Add reference Many (this model) to one @p reference
		void addReferenceManyToOne( const QByteArray& propertyName, 
				const QEOrmModelShd& reference);

		// Find utils	
		QEOrmColumnDefShd findColumnDef( const findByPropertyName& pn) const noexcept;
		QEOrmColumnDefShd findColumnDef( const findByColumnName& cn) const noexcept;
		QEOrmColumnDefShd findColumnDef( const findByAutoIncrement& ) const noexcept;
		QEOrmColumnDefShd findColumnDef( FindColDefPredicate&& predicate) const noexcept;
		const QEOrmForeignDefShd& findForeignTo( const QEOrmModelShd& model) const noexcept;	

	protected:
		QEOrmModel( const QEOrmModel& ) = delete;
		QEOrmModel& operator=( const QEOrmModel& ) = delete;
		
	private:
		void parseAnnotations( const QMetaObject* meta);
		
	private:
		QString m_table;
		QEOrmColumnDefList m_columnDefs;
		QEOrmColumnDefList m_primaryKeyDef;

		QEOrmForeignDefList m_referencesManyToOneDefs;

		QEOrmModelPrivate *d_ptr;
		Q_DECLARE_PRIVATE(QEOrmModel)
};

QE_END_NAMESPACE
