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
 *
 * GNU Lesser General Public License Usage
 * Alternatively, this file may be used under the terms of the GNU Lesser
 * General Public License version 3 as published by the Free
 * Software Foundation and appearing in the file LICENSE.LGPLv3 
 * included in the packaging of this file. Please review the
 * following information to ensure the GNU Lesser General Public License
 * requirements will be met: https://www.gnu.org/licenses/lgpl.html and
 * http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
 *
 * $QE_END_LICENSE$
 */
#pragma once
#include <qe/orm/sql/generator/AbstractGenerator.hpp>
#include <mutex>
#include <array>

namespace qe { namespace orm { namespace sql {
	class CachedGeneratorPrivate;

	/// @todo Cache foreign keys queries
	/// @todo Cache queries using entity list.
	class CachedGenerator : public AbstractGenerator
	{
		public:
			struct Model2StmtValue {
				const entity::Model *model;
				QString cachedValue;
			};
			using Model2Stmt = std::vector< Model2StmtValue>;

			explicit CachedGenerator( AbstractGenerator* generator);
			~CachedGenerator(); 

			// Cached methods
			QString existsStatement( const entity::Model& model) const override;
			QString updateStatement( const qe::entity::Model& model) const override;
			QString insertStatement( const qe::entity::Model& model) const override;
			QString selectionUsingPrimaryKey( 
					const qe::entity::Model& model) const override;
			QString deleteStatement( 
				const entity::Model& model) const override; 
	
			// No cached	
			QString createTableIfNotExistsStatement( 
					const qe::entity::Model& model) const override;
			QString selectionUsingForeignKey( const qe::entity::Model& model, 
					const qe::entity::RelationDef& fkDef) const override;
			QString selectionUsingProperties( const entity::Model& model,
					const entity::EntityDefList& entities) const override;

		protected:
			// Cached methods
			QString projection( const qe::entity::Model &model) const override;
			QString primaryKeyWhereClausure( 
					const qe::entity::Model &model) const override;

			// No cached
			QString foreignKeyWhereClausure( 
					const qe::entity::RelationDef& fkDef) const override;
			QString whereClausure( 
					const qe::entity::EntityDefList &colDefList) const override;
			QString autoIncrementKeyWord() const override;

			QString makeColumnDefinition( 
					const entity::Model& model,
					const qe::entity::EntityDef& column) const override;
			QString makePrimaryKeyDefinition(
					const entity::Model &model) const override;
			QString makeForeignKeyDefinition( 
					const entity::Model& model) const override;
			QString databaseType( 
				const entity::EntityDef& eDef) const override;
			QString databaseEnumerationType( 
				const entity::EntityDef& eDef) const override;

			CachedGeneratorPrivate *d_ptr;

		private:
			enum CachedQuery {
				cqExistStatement = 0,
				cqUpdateStatement,
				cqInsertStatement,
				cqSelectionUsingPrimaryKey,
				cqProjection,
				cqPrimaryKeyWhereClausure,
				cqDeleteStatement,
				cqLast
			};

			Model2StmtValue findByModel( const CachedQuery cq, 
				const entity::Model& model, 
				const std::function<QString ()> strGenerator) const; 

			std::unique_ptr<AbstractGenerator> m_generator;	///< Internal statement generator.

			mutable std::array<Model2Stmt, cqLast> m_stmtByModel;	///< Query cache
			mutable std::mutex m_stmtByModelMtx;						///< Query cache mutex

			Q_DECLARE_PRIVATE( CachedGenerator);
	};
}}}
