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
#include <qe/orm/sql/generator/ANSIGenerator.hpp>
#include <QLoggingCategory>

namespace qe { namespace orm {  namespace sql {
	class SQLiteGeneratorPrivate;
	Q_DECLARE_LOGGING_CATEGORY( lcSQLiteGenerator);

	/// @brief It is an specialization of SQL generator for SQLite.
	class SQLiteGenerator : public ANSIGenerator
	{
		public:
			virtual ~SQLiteGenerator();

		protected:
			QString autoIncrementKeyWord() const override;

			QString makeColumnDefinition( 
				const entity::Model& model, 
				const entity::EntityDef& column) const override; 
					
			QString makePrimaryKeyDefinition(
					const entity::Model &model) const override;

			QString databaseType( 
				const entity::EntityDef& eDef) const override;

			QString databaseEnumerationType( 
				const entity::EntityDef& eDef) const override;

		private:
			Q_DECLARE_PRIVATE( SQLiteGenerator);
	};
}}}
