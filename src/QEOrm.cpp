#include <QEOrm.hpp>
#include <DBDriver/SQliteGenerator.hpp>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QSqlField>
#include <QMetaObject>
#include <QDebug>
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
	
	QSqlQuery executeSQLOrThrow( const QString& stmt, const QString& errorMsg )
	{
		qDebug() << "QEOrm SQL statement: " << stmt;
		QSqlQuery sqlQuery( stmt);
		const QSqlError sqlError = sqlQuery.lastError();
		if( sqlError.type() != QSqlError::NoError )
		{
			const QString msg =  errorMsg
				.arg( sqlError.nativeErrorCode())
				.arg( sqlError.text());
			throw runtime_error( msg.toStdString());
		}
		return sqlQuery;
	}
	
	void executeSQLOrThrow( const QStringList& stmtList, const QString& errorMsg)
	{
		for( const QString& stmt: stmtList)
			executeSQLOrThrow( stmt, errorMsg);
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
}

void QEOrm::save(QObject *const source) const
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
	const QMetaObject *mo = target->metaObject();
	QEOrmModel model = getModel(mo);
	
	QSqlQuery query = executeSQLOrThrow(
		m_sqlGenerator->generateLoadObjectFromDBStmt( pk, model),
		QString( "QEOrm cannot load object from database %1: %2"));
	
	if( !query.next())
		throw runtime_error( "QEOrm cannot load object");
	
	QSqlRecord record = query.record();
	for( int i = 0; i < record.count(); ++i)
	{
		const QSqlField field = record.field( i);
		const QEOrmColumnDef colDef = model.findColumnByName( field.name());
		const QString valueStr = field.value().toString();
		target->setProperty( colDef.propertyName().constData(), 
							 field.value());
	}
}

void QEOrm::checkAndCreateDBTable( const QEOrmModel& model) const
{
	const bool isAlreadyChecked = existsOrCreateUsingDoubleCheckeLocking( m_cachedCheckedTables, model.table(), m_cachedCheckedTablesMtx);
	if( !isAlreadyChecked)
	{
		const QStringList sqlCommands =  m_sqlGenerator->createTablesIfNotExist( model); 
		executeSQLOrThrow( sqlCommands, QString("QEOrm cannot create table '%1' due to error %2: %3")
				.arg( model.table()));
	}
}

bool QEOrm::existsObjectOnDB(const QObject *source, const QEOrmModel &model) const
{
	QSqlQuery query = executeSQLOrThrow( m_sqlGenerator->generateExistsObjectOnDBStmt( source, model),
										 QString("QEOrm cannot check existance of object into db %1: %2"));
	return query.next();
}

void QEOrm::insertObjectOnDB(QObject *source, const QEOrmModel &model) const
{
	QSqlQuery query = executeSQLOrThrow( 
		m_sqlGenerator->generateInsertObjectStmt( source, model),
		QLatin1Literal("QEOrm cannot insert a new object into database %1: %2"));

	// Update object
	QVariant insertId = query.lastInsertId();
	if( ! insertId.isNull())
	{
		const QEOrmColumnDef colDef = model.findAutoIncrementColumn();
		if( colDef.isValid())
			source->setProperty( colDef.propertyName().constData(), insertId);
	}
}

void QEOrm::updateObjectOnDB(const QObject *source, const QEOrmModel &model) const
{
	executeSQLOrThrow(
		m_sqlGenerator->generateUpdateObjectStmt( source, model),
		QLatin1Literal("QEOrm cannot update an object from database %1: %2"));
}

