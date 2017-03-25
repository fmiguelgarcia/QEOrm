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
#include "ResultSet.hpp"
#include <qe/common/Exception.hpp>
#include <qe/entity/Model.hpp>
#include <qe/entity/ModelRepository.hpp>
#include <qe/orm/LoadHelper.hpp>

#include <QStringBuilder>
#include <QSqlRecord>
using namespace qe::orm;
using namespace qe::common;
using namespace qe::entity;

QObject* ResultSetIteratorBase::createInstance( const QMetaObject* mo,
		QObject* parent) const
{
	QObject * o = mo->newInstance( Q_ARG( QObject*, parent));
	if( !o)
	{
		Exception::makeAndThrow(
			QString( "QE ORM cannot create an object of type '%1'" 
				"using the constructor %1:%1( QObject* parent)" )
				.arg( mo->className()));
	}
	return o;
}
			
void ResultSetIteratorBase::loadFromQuery( QObject *o, QSqlQuery& query) const
{
	ModelShd model = ModelRepository::instance().model( o->metaObject());
	if( !model)
	{
		Exception::makeAndThrow(
			QStringLiteral( "QE Orm cannot get the model for object type ")
			% o->metaObject()->className());
	}

	LoadHelper loader;
	loader.loadObjectFromRecord( *model, query.record(), o);
}
