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
#include <QEOrmModel.hpp>

/// @brief This class generates all SQL statement required by QEOrm. 
///
/// It SHOULD use ANSI SQL, in order to be a general SQL generator. 
/// @todo Add table version as a comment or other table, add version to CLASSINFO
class SQLGenerator
{
	public:
		struct TableStatement 
		{
			QString tableName;
			QString sqlStatement;
		};
		using TableStatementList = std::vector<TableStatement>;
		
		
		/// @brief It generates the list of SQL statements to create @p model.
		virtual TableStatementList createTablesIfNotExist( const QEOrmModelShd&  model) const;
	
		/// @brief It generates a SQL statement to check if @p o already exists on mode @p model
		virtual QString generateExistsObjectOnDBStmt( const QObject* o, const QEOrmModel& model) const;
		
		/// @brief It generates an SQL statement to insert @p o in database using @p model.
		virtual QString generateInsertObjectStmt( const QObject *o, const QEOrmModel &model) const;
		
		/// @brief It generates an SQL statement to update fields of @p o.
		virtual QString generateUpdateObjectStmt( const QObject *o, const QEOrmModel& model) const;
		
		/// @brief It generates an SQL statement to load an object from @p model, using 
		/// using @p pk as values for primary key.
		virtual QString generateLoadObjectFromDBStmt( const QVariantList& pk, const QEOrmModel &model) const;

	protected:
		/// @brief It creates the SQL statement for @p model.
		virtual QString createTableIfNotExist( const QEOrmModel&  model) const;
	
		/// @brief It generates the SQL partial statement for the column definition @p column. 
		virtual QString generateColumnDefinition( const QEOrmColumnDef& column) const;
		
		/// @brief It generates the SQL partial statement for primary key on @p model  
		virtual QString generatePrimaryKeyDefinition( const QEOrmModel& model) const;
		
		/// @brief It generate the SQL partial statement for foreign key on @p model.
		virtual QString generateForeignKeyDefinition( const QEOrmModel& model) const;

		/// @brief It generates the SQL partial WHERE statement to locate an object using 
		///  its primary key into @p model.
		virtual QString generateWhereClause( const QEOrmModel &model) const;
	
		/// @brief It maps the C++ type @p propertyType into a database type.
		/// @param size In case of strings, you can limit the number of characters. By default
		///  it is 0, which means no limitation at all.
		virtual QString getDBType( const QMetaType::Type propertyType, const uint size = 0) const;
	
		/// @brief How to defined a column as an Auto-Increment.
		/// 
		/// Different database engines define this in several ways. Overloaded generators
		/// should override this function.
		virtual QString autoIncrementKeyWord() const;
};
