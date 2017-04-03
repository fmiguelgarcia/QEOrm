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
#include <qe/entity/Types.hpp>

namespace qe { namespace orm { namespace sql {

	/// @brief This class generates all SQL statement required by QEOrm. 
	class AbstractGenerator
	{
		friend class CachedGenerator;
		public:
			virtual ~AbstractGenerator();

			/// @brief It generates a SQL statement to check if an object exits
			/// in database. 
			virtual QString existsStatement( const entity::Model& model) const = 0;

			/// @brief It generates an SQL statement to update. 
			virtual QString updateStatement( const entity::Model& model) const = 0;

			/// @brief It generates an SQL statement to insert @p o in 
			/// database using @p model.
			virtual QString insertStatement( const entity::Model& model) const = 0;

			/// @brief It creates the SQL statement for @p model.
			virtual QString createTableIfNotExistsStatement( 
					const entity::Model& model) const = 0;

			/// @brief It generates an SQL statement to load an object from @p model, 
			/// using @p pk as values for primary key.
			virtual QString selectionUsingPrimaryKey( 
					const entity::Model& model) const = 0;

			/// @brief It creates an SQL statement to load objects using @p fk foreign
			/// key in model @p model.
			virtual QString selectionUsingForeignKey( const entity::Model& model, 
					const entity::RelationDef& fkDef) const = 0;

			/// @brief It generates a SQL select statement using @p entities as
			///  where clause.
			virtual QString selectionUsingProperties( const entity::Model& model,
					const entity::EntityDefList& entities) const = 0;

			/// @brief It generates an SQL delete statement for a object
			///	using its primary key.
			virtual QString deleteStatement( 
				const entity::Model& model) const = 0;

		protected:
			/// @brief It return the projection (SELECT clause) for 
			/// specific model
			virtual QString projection( const entity::Model &model) const = 0;

			/// @brief It generates the SQL partial WHERE statement to 
			/// locate an object using its primary key into @p model.
			virtual QString primaryKeyWhereClausure( 
					const entity::Model &model) const = 0;

			/// @brief It generates the SQL partial WHERE statement to
			/// locate an object using a foreign key.
			virtual QString foreignKeyWhereClausure( 
					const entity::RelationDef& fkDef) const = 0;

			virtual QString whereClausure( 
					const entity::EntityDefList &colDefList) const = 0;

			/// @brief How to defined a column as an Auto-Increment.
			/// 
			/// Different database engines define this in several ways. 
			/// Overloaded generators should override this function.
			virtual QString autoIncrementKeyWord() const = 0;

			/// @brief It generates the SQL partial statement for the 
			/// column definition @p column. 
			virtual QString makeColumnDefinition( 
				const entity::Model& model,
				const entity::EntityDef& column) const = 0;

			/// @brief It generates the SQL partial statement for primary.
			virtual QString makePrimaryKeyDefinition(
					const entity::Model &model) const = 0;

			/// @brief It generates the SQL partial statement for foreign keys.
			virtual QString makeForeignKeyDefinition( 
					const entity::Model& model) const = 0;

			/// @brief It maps the C++ type @p propertyType into a database type.
			/// @param size In case of strings, you can limit the 
			/// number of characters. By default it is 0, which means no 
			/// limitation at all.
			virtual QString databaseType( 
					const entity::EntityDef& eDef) const = 0;

			/// @brief It generates an enumeration type.
			virtual QString databaseEnumerationType( 
					const entity::EntityDef& eDef) const = 0;
			
	};
}}}	
