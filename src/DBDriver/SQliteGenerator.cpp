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

#include "SQliteGenerator.hpp"

QString SQliteGenerator::generateColumnDefinition( const QEOrmColumnDef& column) const
{
	QString sqlColumnDef;
	if( !column.isDbAutoIncrement)
			sqlColumnDef = SQLGenerator::generateColumnDefinition( column);
	else
		sqlColumnDef = QString( "'%1' INTEGER PRIMARY KEY %2 ")
			.arg( column.dbColumnName)
			.arg( autoIncrementKeyWord());

	return sqlColumnDef;
}

/// @note In SQLite, if a colum is AUTOINCREMENT then this column has to be PRIMARY KEY.
QString SQliteGenerator::generatePrimaryKeyDefinition(const QEOrmModel &model) const
{
	QString sqlStmt;
	
	const auto autoIncrementDef = model.findColumnDef( QEOrmModel::findByAutoIncrement{});
	if( ! autoIncrementDef) 
		sqlStmt = SQLGenerator::generatePrimaryKeyDefinition(model);
	
	return sqlStmt;
}


QString SQliteGenerator::autoIncrementKeyWord() const
{ return QLatin1Literal("AUTOINCREMENT"); }
