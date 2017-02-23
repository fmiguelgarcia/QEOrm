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

#include <QSharedData>
#include <QEOrmColumnDef.hpp>
#include <QEOrmForeignDef.hpp>
#include <vector>
#include <functional>
#include <map>

QE_BEGIN_NAMESPACE

class QEAnnotationModel;
class QEOrmModelPrivate : public QSharedData
{
	public:
		QEOrmModelPrivate( const QEAnnotationModel& model, const QMetaObject* metaObject);
		QEOrmModelPrivate(const QEOrmModelPrivate &other);

		QString table() const noexcept;

		const std::vector<QEOrmColumnDef>& columns() const noexcept;
		const std::vector<QEOrmColumnDef>& primaryKey() const noexcept;
		const std::vector<QEOrmForeignDef>& referencesToOne() const noexcept;
		void addReferencesToOne( const QEOrmForeignDef& fkDef);

		QEOrmColumnDef findColumnDefIf( const std::function<bool(const QEOrmColumnDef&)> predicate) const;

	private:
		void parseAnnotations( const QEAnnotationModel& model, const QMetaObject* metaObject); 
		std::vector<QEOrmColumnDef> parseAnnotationsGetPrimaryKeys( const QEAnnotationModel& model) const;
		std::vector<QEOrmColumnDef> generateNoPrimaryKey() const;

	private:
		QString m_table;
		std::vector<QEOrmColumnDef> m_columns;
		
		std::vector<QEOrmColumnDef> m_primaryKey;
		std::vector<QEOrmForeignDef> m_referencesToOne;
};

QE_END_NAMESPACE
