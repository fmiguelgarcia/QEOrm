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
#include "FindHelper.hpp"
#include <qe/orm/sql/Executor.hpp>
#include <qe/orm/sql/generator/AbstractGenerator.hpp>
#include <qe/orm/sql/GeneratorRepository.hpp>
#include <qe/orm/SerializedItem.hpp>

#include <qe/entity/Model.hpp>
#include <QSqlQuery>

using namespace qe::orm;
using namespace qe::entity;
using namespace std;

FindHelper::~FindHelper()
{}

QSqlQuery FindHelper::findEqualProperty(
	const	Model& model,
	const SerializedItem* const source,
	const map<QString, QVariant>& properties) const
{
	QSqlQuery ds;

	EntityDefList entityList;
	QVariantList values;

	for( const auto& propItem: properties)
	{
		EntityDefShd eDef = model.findEntityDef( 
				Model::findByPropertyName{ propItem.first.toLocal8Bit()});
		if( eDef )
		{
			entityList.push_back( eDef);
			values.push_back( propItem.second);
		}
	}

	const sql::Executor& sqlExec = source->executor();
	sql::AbstractGenerator* stmtMaker = sql::GeneratorRepository::instance().generator( 
		sqlExec.dbmsType());
	const QString stmt = stmtMaker->selectionUsingProperties(
		model, entityList);

	ds = sqlExec.execute( stmt, values,
		QStringLiteral("QE Orm Find Helper cannot execute query.")); 
	
	return ds;
}

