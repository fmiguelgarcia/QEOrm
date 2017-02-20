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
#include "QEOrmModelPrivate.hpp"
#include <QEAnnotation/QEAnnotation.hpp>
#include <QEOrmColumnDef.hpp>

QE_BEGIN_NAMESPACE
class QEOrmModel : public QEAnnotationModel
{
	public:
		explicit QEOrmModel( const QMetaObject* meta);
		QEOrmModel( const QEOrmModel& ) noexcept;
		QEOrmModel& operator=( const QEOrmModel& ) noexcept;
	
		// Table
		QString table() const noexcept;
		std::vector<QEOrmColumnDef> primaryKey() const noexcept;
		std::vector<QEOrmColumnDef> noPrimaryKey() const noexcept;
		
		QEOrmColumnDef columnByProperty( const QString & property) const noexcept;
		QEOrmColumnDef columnByName( const QString & columnName) const noexcept;
		QStringList columnNames() const;
		QString autoIncrementColumnName() const;
		
	private:
		void parseAnnotations( const QMetaObject* meta);
		QStringList parseAnnotationsGetPrimaryKeys() const;
		
	private:
		QSharedDataPointer<QEOrmModelPrivate> d_ptr;
};
Q_DECLARE_TYPEINFO( QEOrmModel, Q_MOVABLE_TYPE);

QE_END_NAMESPACE