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
#include <qe/orm/Global.hpp>
#include <qe/entity/Types.hpp>
#include <QLoggingCategory>
#include <QVariantList>
#include <QSqlQuery>
#include <memory>

namespace qe { namespace orm { namespace sql 
{
	Q_DECLARE_LOGGING_CATEGORY( lcExecutor);
	class Generator;
	class ExecutorPrivate;

	class QEORM_EXPORT Executor
	{
		public:
			/// @param connName SQL database connection name. If it is empty, 
			/// the default connection will be used.
			explicit Executor( const QString& connName = QString());
			virtual ~Executor();

			int dbmsType() const noexcept;

			QSqlQuery execute( 
					const QString& stmt, 
					const QVariantList& params,
					const QString& errorMsg) const;
			QSqlQuery execute( 
					QSqlQuery& query, 
					const QString& errorMsg) const;
			QSqlQuery execute( 
					qe::entity::ObjectContext& context, 
					const qe::entity::Model& model,
					const QString& stmt, 
					const QObject* source, 
					const QString& errorMsg) const;

		protected:
			void logQuery( QSqlQuery& query) const;

			ExecutorPrivate *d_ptr;

		private:
			QString m_connName;
			int m_dbmsType;

			Q_DECLARE_PRIVATE( Executor);
	};
}}}

