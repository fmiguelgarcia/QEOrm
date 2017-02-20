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
#include <QEOrmModel.hpp>

/// @todo Add table version as a comment or other table, add version to CLASSINFO
class SQLGenerator
{
	public:
		virtual QString createTableIfNotExist( const QEOrmModel&  model) const;
		
		virtual QString generateExistsObjectOnDBStmt( const QObject* o, const QEOrmModel& model) const;
		virtual QString generateInsertObjectStmt( const QObject *o, const QEOrmModel &model) const;
		virtual QString generateUpdateObjectStmt( const QObject *o, const QEOrmModel& model) const;
		virtual QString generateLoadObjectFromDBStmt( const QVariantList& pk, const QEOrmModel &model) const;

	protected:
		virtual QString generateColumnDefinition( const QEOrmModel& model, const QString& column) const;
		virtual QString generatePrimaryKeyDefinition( const QEOrmModel& model) const;
		virtual QString generateWhereClause(const QObject *o, const QEOrmModel& model) const;
		virtual QString generateWhereClause( const QVariantList& pk, const QEOrmModel &model) const;
		
		virtual QString getDBType( const QMetaType::Type propertyType, const uint size) const;
		virtual QString variantToSQL( const QVariant& value, const int propertyType) const;
	
		// Specific keywords
		virtual QString autoIncrementKeyWord() const;
};
