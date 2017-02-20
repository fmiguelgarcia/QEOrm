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
#include <vector>

QE_BEGIN_NAMESPACE

class QEAnnotationModel;
class QEOrmModelPrivate : public QSharedData
{
	public:
		using ColumnDefBy = std::map<QString, QEOrmColumnDef>;
	
		QEOrmModelPrivate();
		QEOrmModelPrivate(const QEOrmModelPrivate &other);
		void parseAnnotations( const QEAnnotationModel* model, const QMetaObject* metaObject); 
		std::vector<QEOrmColumnDef> parseAnnotationsGetPrimaryKeys( const QEAnnotationModel* model) const;
		std::vector<QEOrmColumnDef> generateNoPrimaryKey() const;

		QString table;
		std::vector<QEOrmColumnDef> primaryKey;
		std::vector<QEOrmColumnDef> noPrimaryKey;
		ColumnDefBy columnsByProperty;
		ColumnDefBy columnsByName;
};

QE_END_NAMESPACE