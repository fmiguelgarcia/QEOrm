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
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QSqlField>
#include <QMetaObject>
#include <QDebug>
#include <utility>
#include <map>
#include <stack>
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

	void debugExecutedQuery( QSqlQuery& query)
	{
		QString message;
		QTextStream os( &message);
		const QMap<QString, QVariant> boundValues = query.boundValues();
		
		os << QLatin1Literal( "QE Orm ") << endl
			<< QLatin1Literal( "   + query: ") << query.executedQuery() << endl
			<< QLatin1Literal( "   + parameters: { ");
		for( const QString boundKey : boundValues.keys())
			os << QLatin1Char('{') << boundKey << QLatin1Literal(", ") << boundValues[boundKey].toString() << QLatin1Literal("} ");
		os << QLatin1Char('}') << endl;
		
		qDebug() << message;
	}

	QSqlQuery executeSqlOrThrow( QSqlQuery& query, const QString& errorMsg)
	{
		const bool isSuccess = query.exec();
		debugExecutedQuery( query);
		
		if( !isSuccess)
		{
			const QSqlError sqlError = query.lastError();
			if( sqlError.type() != QSqlError::NoError )
			{
				const QString msg =  errorMsg
					.arg( sqlError.nativeErrorCode())
					.arg( sqlError.text());
				throw runtime_error( msg.toStdString());
			}
		}
		return query;
	}
	
	QSqlQuery executeSqlOrThrow( const QVariantList& bindValues, 
			const QString& stmt, const QString& errorMsg)
	{
		QSqlQuery query;
		query.prepare( stmt);
		for( int i = 0; i < bindValues.size(); ++i)
			query.bindValue( i, bindValues[i]);
		
		return executeSqlOrThrow( query, errorMsg);
	}
	
	QSqlQuery executeSqlOrThrow( const QObject* object, 
			const stack<QObject*> &context, const QEOrmModel& model, 
			const QString& stmt, const QString& errorMsg )
	{
		QSqlQuery query;
		query.prepare( stmt);
		for( const auto& colDef: model.columnDefs())
		{
			if( colDef->mappingType == QEOrmColumnDef::MappingType::NoMappingType)
			{
				QVariant value = object->property( colDef->propertyName);
				if( colDef->isDbAutoIncrement && value.toInt() == 0)
					value = QVariant();
				query.bindValue( QString(":%1").arg( colDef->dbColumnName), value);
			}
		}

		if( !context.empty())
		{
			QObject *contextTopObject = context.top();
			for( const auto& fkDef : model.referencesManyToOneDefs())
			{
				const auto& foreignKeys = fkDef->foreignKeys();
				const auto& refKeys = fkDef->reference()->primaryKeyDef();
				for( uint i = 0; i < foreignKeys.size(); ++i) 
				{
					const QVariant value = contextTopObject->property( 
							refKeys[i]->propertyName); 

					query.bindValue( 
						QString( ":%1").arg( foreignKeys[i]->dbColumnName),
						value);
				}
			}
		}
	
		return executeSqlOrThrow( query, errorMsg);
	}

	void executeSqlOrThrow( const QStringList& stmtList, const QString& errorMsg)
	{
		for( const QString& stmt: stmtList)
		{
			QSqlQuery query;
			query.prepare( stmt);
			executeSqlOrThrow( query, errorMsg);
		}
	}


	struct ScopeStackedContext 
	{
		ScopeStackedContext( QObject* obj, stack<QObject*>& context)
			:m_context( context)
		{ context.push( obj); }

		~ScopeStackedContext()
		{ m_context.pop();}

		stack<QObject*> &m_context;
	};
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

void QEOrm::save(QObject *const source, stack<QObject*> context) const
{
	const QMetaObject *mo = source->metaObject();
	QEOrmModelShd model = getModel(mo);

	if( model )
	{
		checkAndCreateDBTable( model);
		if( existsObjectOnDB( source, *model))
			updateObjectOnDB( source, context, *model);
		else
			insertObjectOnDB( source, context, *model);

		saveOneToMany( source, context, model);
	}
}


void QEOrm::saveOneToMany(QObject *const source, 
		stack<QObject*>& context, const QEOrmModelShd& model) const
{
	ScopeStackedContext _( source, context);
	for( const auto& colDef : model->columnDefs())
	{
		if( colDef->mappingType == QEOrmColumnDef::MappingType::OneToMany)
		{
			const QByteArray& propertyName = colDef->propertyName;
			const QVariant propertyValue = source->property( propertyName);
			if( ! propertyValue.canConvert<QVariantList>())
				throw runtime_error( 
					QString("QE Orm can only use QVariantList for mapping property %1") 
						.arg( QString(propertyName)).toStdString());

			QVariantList values = propertyValue.toList();
			for( QVariant& value : values)
			{
				QObject *refItem = value.value<QObject*>();
				if( refItem )
					save( refItem, context);
			}
			qDebug() << "Property " << propertyName << " has " << values.size() << " items.";
		}
	}
}

void QEOrm::load(const QVariantList pk, QObject *target) const
{
	const QMetaObject *mo = target->metaObject();
	QEOrmModelShd model = getModel(mo);
	if( !model)
		return;

	QSqlQuery query = executeSqlOrThrow(
		pk,
		m_sqlGenerator->generateLoadObjectFromDBStmt( pk, *model),
		QString( "QEOrm cannot load object from database %1: %2"));
	
	if( !query.next())
		throw runtime_error( "QEOrm cannot load object");
	
	QSqlRecord record = query.record();
	for( int i = 0; i < record.count(); ++i)
	{
		const QSqlField field = record.field( i);
		const auto colDef = model->findColumnDef( 
				QEOrmModel::findByColumnName{ field.name()});
		if( colDef )
		{
			target->setProperty( colDef->propertyName.constData(), 
							 field.value());
		}
	}

	/// @todo Load one to many
//	loadOneToMany( target, context, model);
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
		
		executeSqlOrThrow( sqlCommands, QString("QEOrm cannot create table '%1' due to error %2: %3")
				.arg( model->table()));
	}
}

bool QEOrm::existsObjectOnDB(const QObject *source, const QEOrmModel &model) const
{
	QVariantList pkValues;
	for( const auto& colDef: model.primaryKeyDef())
	{
		const QVariant value = source->property( colDef->propertyName);
		if( colDef->isDbAutoIncrement && value.toInt() == 0)
			pkValues << QVariant();
		else
			pkValues << value;
	}
	
	QSqlQuery query = executeSqlOrThrow( 
		pkValues,
		m_sqlGenerator->generateExistsObjectOnDBStmt( source, model),
		QString("QEOrm cannot check existance of object into db %1: %2"));
	return query.next();
}

void QEOrm::insertObjectOnDB(QObject *source, const stack<QObject*>& context, 
		const QEOrmModel &model) const
{
	QSqlQuery query = executeSqlOrThrow( 
		source, context, model, 
		m_sqlGenerator->generateInsertObjectStmt( source, model),
		QLatin1Literal("QEOrm cannot insert a new object into database %1: %2"));

	// Update object
	QVariant insertId = query.lastInsertId();
	if( ! insertId.isNull())
	{
		const auto colDef = model.findColumnDef( QEOrmModel::findByAutoIncrement{});
		if( colDef)
			source->setProperty( colDef->propertyName.constData(), insertId);
	}
}

void QEOrm::updateObjectOnDB(const QObject *source, 
		const stack<QObject*>& context, const QEOrmModel &model) const
{
	executeSqlOrThrow(
		source, context, model,
		m_sqlGenerator->generateUpdateObjectStmt( source, model),
		QLatin1Literal("QEOrm cannot update an object from database %1: %2"));
}

