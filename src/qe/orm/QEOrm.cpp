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
#include "QEOrm.hpp"
#include "SaveHelper.hpp"
#include "LoadHelper.hpp"
#include "serialization/SerializedItem.hpp"
#include <qe/common/Exception.hpp>
#include <qe/entity/Model.hpp>
#include <qe/entity/ModelRepository.hpp>
#include <QStringList>
#include <QStringBuilder>

using namespace qe::orm;
using namespace qe::entity;
using namespace qe::common;
using namespace std;

namespace {
	/// @brief Utility to use double check locking and create/insert objects
	/// into a container.
	template< typename C, typename K, typename M>
	bool existsOrCreateUsingDoubleCheckeLocking( C &&container, K&& key, M&& mutex)
	{
		auto itr = container.find( key);
		if( itr == std::end( container))
		{
			std::lock_guard<M> _( std::forward<M>(mutex));
			itr = container.find( key);
			if( itr == std::end( container))
			{
				container.insert( key);
				return false;
			}
		}
		return true;
	}

	template< class T, class M>
	void checkAndCreateDatabaseTables(
			const qe::orm::SaveHelper& saveHelper,
			const ModelShd& model,
			qe::orm::SerializedItem* const target,
			T& checkedTables,
			M& checkedTablesMtx) 
	{
		const bool isAlreadyChecked = existsOrCreateUsingDoubleCheckeLocking( 
				checkedTables, model->name(), checkedTablesMtx);
		if( !isAlreadyChecked)
		{
			const QStringList tables = saveHelper.createTables( model, target);
			for( const QString& table: tables)
				existsOrCreateUsingDoubleCheckeLocking(
					  	checkedTables, table, checkedTablesMtx);
		}
	}

	qe::orm::SerializedItem* checkedCast( 
			AbstractSerializedItem* const item)
	{
		using namespace qe::orm::sql;
		SerializedItem* const ormItem = dynamic_cast<SerializedItem*>( item);

		if( !ormItem)
			Exception::makeAndThrow( 
				QStringLiteral( "Orm serializer only supports %1 as serialized item")
					.arg( typeid( SerializedItem).name()));
			
		return ormItem;
	}	
}

QEOrm &QEOrm::instance()
{
	static unique_ptr<QEOrm> instance;
	static once_flag onceFlag;

	call_once( onceFlag,
			   []{ instance.reset( new QEOrm);});
	return *instance.get();
}

QEOrm::QEOrm()
{}

void QEOrm::save( QObject* const source) const
{
	sql::Executor helper( QLatin1String( QSqlDatabase::defaultConnection));
	SerializedItem si( helper);

	AbstractSerializer::save( source, &si); 
}

void QEOrm::save( ObjectContext& context, const ModelShd& model, 
		QObject *const source, AbstractSerializedItem* const target) const
{
	using namespace qe::orm::sql;
	SerializedItem* const ormTarget = checkedCast( target);

	SaveHelper saver;
	checkAndCreateDatabaseTables( saver, model, ormTarget, m_checkedTables, 
			m_checkedTablesMtx);
	saver.save( context, model, source, ormTarget);
}


void QEOrm::load( ObjectContext& context, const ModelShd& model, 
	const AbstractSerializedItem *const source, QObject *const target) const
{
	using namespace qe::orm::sql;
	
	if( !source)
		Exception::makeAndThrow(
			QStringLiteral("QE Orm does not support null source in loads"));
	
	std::unique_ptr<SerializedItem> ormSourceAutoGenerated;
	const SerializedItem* ormSource = dynamic_cast<const SerializedItem*> ( source);

	// Transform to SerializedItem using default connection.
	if( !ormSource)
	{
		ormSourceAutoGenerated.reset( new SerializedItem( source->primaryKey())); 
		ormSource = ormSourceAutoGenerated.get();
	}

	LoadHelper loader;
	loader.load( context, model, ormSource, target);
}

void QEOrm::load(QVariantList&& primaryKey, QObject*const target) const
{
	std::unique_ptr<const AbstractSerializedItem> ormSource( 
		new SerializedItem( std::move(primaryKey)));
	
	AbstractSerializer::load( ormSource.get(), target);
}

ModelShd QEOrm::getModelOrThrow( const QMetaObject* metaObject) const
{
	ModelShd model = ModelRepository::instance().model( metaObject);
	if( !model)
		Exception::makeAndThrow(
				QStringLiteral( "QE Orm cannot find model for class ")
				% metaObject->className());

	return model;
}

