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
#include <QEOrm.hpp>
#include <DBDriver/SQliteGenerator.hpp>
#include <helpers/QEOrmLoadHelper.hpp>
#include <helpers/QEOrmSaveHelper.hpp>
#include <QSqlDatabase>
#include <utility>
#include <map>
#include <stack>
#include <mutex>

using namespace std;
QE_USE_NAMESPACE

namespace {

	/// @brief Utility to use double check locking and create/insert objects
	/// into a map.
	template <typename V,  typename K, typename M, typename F>
	V findOrCreateUsingDoubleCheckLocking( 
		std::map<K,V>& container, 
		K& key, 
		M& mutex, 
		const F& createFunc )
	{
		auto itr = container.find( key);
		if( itr == std::end( container))
		{
			std::lock_guard<M> _( mutex);
			itr = container.find( key);
			if( itr == std::end( container))
			{
				V value = createFunc(); 
				itr = container.insert(
					std::make_pair( key, value) ).first;
			}
		}
		return itr->second;
	}
	
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


	/// @brief It creates a new SQLGenerator based on current default DB driver.
	SQLGenerator* getSQLGeneratorForDefaultDB()
	{
		map<QString, std::function< SQLGenerator*()> > sqlGenMaker = {
			{ "", [](){ return new SQLGenerator;}},
			{"QSQLITE", [](){ return new SQliteGenerator;}} 
		};
		
		QSqlDatabase db = QSqlDatabase::database( 
			QLatin1String(QSqlDatabase::defaultConnection), 
			false);
		const QString driverName =  db.driverName();
		
		auto itr = sqlGenMaker.find( driverName );
		if( itr == end( sqlGenMaker))
			itr = sqlGenMaker.find( QString(""));
		
		return itr->second();
	}

	void throwCannotFindModelForClass( const QMetaObject* mo )
	{
		throw runtime_error( QString( "QE Orm cannot find model for '%1' class")
				.arg( (mo)?(mo->className()):("unknow class")).toStdString());
	}

}

std::unique_ptr<QEOrm> QEOrm::m_instance;
std::once_flag QEOrm::m_onceFlag;

QEOrm &QEOrm::instance()
{
	call_once( m_onceFlag,
			   []{ m_instance.reset( new QEOrm);});
	return *m_instance.get();
}

QEOrm::QEOrm()
{
	m_sqlGenerator.reset( getSQLGeneratorForDefaultDB());
}

QEOrmModelShd QEOrm::getModel( const QMetaObject* metaObject) const
{
	return findOrCreateUsingDoubleCheckLocking( 
		m_cachedModels, 
		metaObject, 
		m_cachedModelsMtx, 
		[metaObject](){ return make_shared<QEOrmModel>(metaObject);});
}
		
void QEOrm::load( const QVariantList pk, QObject* target) const
{
	const QMetaObject* mo = target->metaObject();
	QEOrmModelShd model = getModel( mo);
	if( !model)
		throwCannotFindModelForClass( mo);

	stack<QObject*> context;
	QEOrmLoadHelper helper( m_sqlGenerator.get());
	helper.sqlHelper.isShowQueryEnabled = true;
	helper.load( pk, model, target, context);
}

void QEOrm::save(QObject *const source) const
{
	const QMetaObject *mo = source->metaObject();
	QEOrmModelShd model = QEOrm::instance().getModel(mo);
	if( !model )
		throwCannotFindModelForClass( mo);
		
	checkAndCreateDBTable( model);

	stack<QObject*> context;
	QEOrmSaveHelper helper( m_sqlGenerator.get());
	helper.sqlHelper.isShowQueryEnabled = true;
	helper.save( source, model, context);
}

void QEOrm::checkAndCreateDBTable( const QEOrmModelShd& model) const
{
	const bool isAlreadyChecked = existsOrCreateUsingDoubleCheckeLocking( 
			m_cachedCheckedTables, model->table(), m_cachedCheckedTablesMtx);
	if( !isAlreadyChecked)
	{
		const auto tableStmtList =  m_sqlGenerator->createTablesIfNotExist( model); 
	
		QStringList sqlCommands;
		for( const auto& ts : tableStmtList)
		{
			sqlCommands << ts.sqlStatement;
			existsOrCreateUsingDoubleCheckeLocking( m_cachedCheckedTables, ts.tableName, m_cachedCheckedTablesMtx);
		}
	
		QEOrmSqlHelper helper;
		helper.isShowQueryEnabled = true;
		helper.execute( sqlCommands, 
				QString("QEOrm cannot create table '%1' due to error %2: %3")
					.arg( model->table()));
	}
}

