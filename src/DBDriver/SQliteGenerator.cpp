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

#include "SQliteGenerator.hpp"

QString SQliteGenerator::generateColumnDefinition(const QEOrmModel &model, const QEOrmColumnDef column) const
{
	QString sqlColumnDef;
	if( !column.isDbAutoIncrement())
			sqlColumnDef = SQLGenerator::generateColumnDefinition(model, column);
	else
		sqlColumnDef = QString( "'%1' INTEGER PRIMARY KEY AUTOINCREMENT")
			.arg( column.dbColumnName());

	return sqlColumnDef;
}

/// @note In SQLite, if a colum is AUTOINCREMENT then this column has to be PRIMARY KEY.
QString SQliteGenerator::generatePrimaryKeyDefinition(const QEOrmModel &model) const
{
	QString sqlStmt;
	
	const QEOrmColumnDef autoIncrementDef = model.findAutoIncrementColumn();
	if( ! autoIncrementDef.isValid()) 
		sqlStmt = SQLGenerator::generatePrimaryKeyDefinition(model);
	
	return sqlStmt;
}


QString SQliteGenerator::autoIncrementKeyWord() const
{ return QLatin1Literal("AUTOINCREMENT"); }
