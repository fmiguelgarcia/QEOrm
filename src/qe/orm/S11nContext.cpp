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
#include "S11nContext.hpp"
#include <qe/orm/sql/GeneratorRepository.hpp>

using namespace qe::orm;
using namespace qe::orm::sql;

S11nContext::S11nContext(
	const QVariantList& pkValues, 
	const QString& connName)
		: AbstractS11nContext( pkValues),
		m_helper( connName)
{}

S11nContext::~S11nContext()
{}

int S11nContext::dbmsType() const noexcept
{ return m_helper.dbmsType();}

QSqlQuery S11nContext::execute( 
		const QString& stmt, 
		const QVariantList& params,
		const QString& errorMsg) const
{
	return m_helper.execute( stmt, params, errorMsg);
}

QSqlQuery S11nContext::execute(
	const qe::entity::Model& model,
	const QString& stmt, 
	const QObject* source, 
	const QString& errorMsg) const
{
	return m_helper.execute( m_context, model, stmt, source, errorMsg);
}

AbstractGenerator* S11nContext::statementMaker() const
{
	return GeneratorRepository::instance()
		.generator( m_helper.dbmsType()); 
}

