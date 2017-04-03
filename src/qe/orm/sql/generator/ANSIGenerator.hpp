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
#include <qe/orm/sql/generator/AbstractGenerator.hpp>
#include <QString>
#include <QVariantList>
#include <vector>
#include <memory>

namespace qe { namespace orm { namespace sql {

	/// @brief This class generates all SQL statement required by QEOrm. 
	///
	/// It SHOULD use ANSI SQL, in order to be a general SQL generator. 
	/// @todo Add table version as a comment or other table, add version to CLASSINFO
	class ANSIGenerator : public AbstractGenerator
	{
		public:
			virtual ~ANSIGenerator();

			QString existsStatement( const entity::Model& model) const override;

			QString updateStatement( const qe::entity::Model& model) const override;

			QString insertStatement( const qe::entity::Model& model) const;

			QString createTableIfNotExistsStatement( 
					const qe::entity::Model& model) const override;

			QString selectionUsingPrimaryKey( 
					const qe::entity::Model& model) const override;
	
			QString selectionUsingForeignKey( const entity::Model& model, 
					const entity::RelationDef& fkDef) const override;

			QString selectionUsingProperties( const entity::Model& model,
					const entity::EntityDefList& entities) const override;

			QString deleteStatement( 
				const entity::Model& model) const override; 


		protected:
			QString projection( const entity::Model &model) const override;

			QString primaryKeyWhereClausure( 
					const entity::Model &model) const override;

			QString foreignKeyWhereClausure( 
					const entity::RelationDef& fkDef) const override;

			QString whereClausure( 
					const entity::EntityDefList &colDefList) const override;

			QString autoIncrementKeyWord() const override;

			QString makeColumnDefinition( 
				const entity::Model& model,
				const entity::EntityDef& column) const override;
			
			QString makePrimaryKeyDefinition(
					const entity::Model &model) const override;
		
			QString makeForeignKeyDefinition( 
					const entity::Model& model) const override;

			QString databaseType( 
				const entity::EntityDef& eDef) const override; 
					
			QString databaseEnumerationType( 
				const entity::EntityDef& eDef) const override;
	};
}}}
