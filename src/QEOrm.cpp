#include <QEOrm.hpp>
#include <DBDriver/SQliteGenerator.hpp>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QMetaObject>
#include <utility>
#include <map>
#include <mutex>

using namespace std;
QE_USE_NAMESPACE

namespace {
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

QEOrmModel QEOrm::getModel( const QMetaObject* metaObject) const
{
	
	return findOrCreateUsingDoubleCheckLocking( 
		m_cachedModels, 
		metaObject, 
		m_cachedModelsMtx, 
		[metaObject](){ return QEOrmModel(metaObject);});
#if 0
	auto itr = m_cachedModels.find( metaObject);
	if( itr == end( m_cachedModels))
	{
		lock_guard<std::mutex> _(m_cachedModelsMtx);
		
		itr = m_cachedModels.insert( 
			make_pair( metaObject, QEOrmModel( metaObject))).first;
	}
			
	return itr->second;
#endif
}

void QEOrm::save(const QObject *const source) const
{
	const QMetaObject *mo = source->metaObject();
	QEOrmModel model = getModel(mo);
	
	checkAndCreateDBTable( model);
	if( existsObjectOnDB( source, model))
		updateObjectOnDB( source, model);
	else
		insertObjectOnDB( source, model);
}

void QEOrm::load(const QVariantList pk, QObject *target) const
{

}

void QEOrm::checkAndCreateDBTable( const QEOrmModel& model) const
{
	const bool isAlreadyChecked = existsOrCreateUsingDoubleCheckeLocking( m_cachedCheckedTables, model.table(), m_cachedCheckedTablesMtx);
	if( !isAlreadyChecked)
	{
		const QString sqlCommand =  m_sqlGenerator->createTableIfNotExist( model); 
		QSqlDatabase db = QSqlDatabase::database();
		QSqlQuery sqlQuery = db.exec( sqlCommand);
		QSqlError sqlError = sqlQuery.lastError();
		if( sqlError.type() != QSqlError::NoError )
		{
			QString msg = QString("QEOrm cannot create table '%1' due to error %2: %3")
				.arg( model.table())
				.arg( sqlError.nativeErrorCode())
				.arg( sqlError.text());
			throw runtime_error( msg.toStdString());
		}
	}
}



bool QEOrm::existsObjectOnDB(const QObject *source, const QEOrmModel &model) const
{
	return false;
}

void QEOrm::insertObjectOnDB(const QObject *source, const QEOrmModel &model) const
{

}

void QEOrm::updateObjectOnDB(const QObject *source, const QEOrmModel &model) const
{

}

