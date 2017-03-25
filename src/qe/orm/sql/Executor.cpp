#include "Executor.hpp"
#include "generator/ANSIGenerator.hpp"
#include "generator/SQLiteGenerator.hpp"
#include <qe/entity/Model.hpp>
#include <qe/entity/EntityDef.hpp>
#include <qe/entity/RelationDef.hpp>
#include <qe/common/Exception.hpp>

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlDriver>
#include <QString>
#include <QStringBuilder>
#include <QVariant>
#include <QDataStream>
#include <QBitArray>
#include <QByteArray>
#include <QDateTime>

#include <map>
#include <functional>

using namespace qe::orm::sql;
using namespace qe::orm;
using namespace qe::entity;
using namespace qe::common;
using namespace std;

void bindNoMappingColumns( const Model& model, QSqlQuery& query, 
		const QObject* source);
void bindMappingOneToMany( const Model& model, QSqlQuery& query,  
	const ObjectContext& context);


Q_LOGGING_CATEGORY( qe::orm::sql::lcExecutor, "com.dmious.qe.orm.sqlHelper");

// Class Generator
// ============================================================================

Executor::Executor( const QString& connName)
	: m_connName( connName)
{
	// If connection name is empty, it will use the default connection.
	if( m_connName.isEmpty())
		m_connName = QLatin1Literal( QSqlDatabase::defaultConnection);
		

	QSqlDatabase db = QSqlDatabase::database( connName, false);
	QSqlDriver* driver = db.driver();
	if( driver)
		m_dbmsType = static_cast<int>( driver->dbmsType());
	else
		m_dbmsType = QSqlDriver::UnknownDbms;
}

Executor::Executor( const Executor& other) = default;
			
Executor::~Executor() = default;
			
int Executor::dbmsType() const
{ return m_dbmsType;}

#if 0
			
QSqlQuery Executor::loadUsingForeignKey( ObjectContext& context, const Model& model, 
		const RelationDef& fkDef, const QObject* target) const
{
	const QString stmt = m_sqlStmtBuilder->selectionUsingForeignKey( model, fkDef);
	QSqlQuery ds = execute( context, model, stmt, target,
		  QStringLiteral( "QE Orm cannot fetch any row from table '")
	  		% model.name()
	 		% QStringLiteral( "' using foreign key"));

	return ds;
}
#endif

// Executor execution
// ============================================================================

QSqlQuery Executor::execute( const QString& stmt, const QVariantList& params,
	const QString& errorMsg) const
{
	QSqlQuery query( QSqlDatabase::database( m_connName));
	query.prepare( stmt);
	for( int i = 0; i < params.size(); ++i)
		query.bindValue( i, params[i]);

	return execute( query, errorMsg);
}

QSqlQuery Executor::execute( QSqlQuery& query, const QString& errorMsg) const
{
	const bool isSuccess = query.exec();
	logQuery( query);

	if( !isSuccess)
	{
		const QSqlError sqlError = query.lastError();
		if( sqlError.type() != QSqlError::NoError )
		{
			Exception::makeAndThrow(
				errorMsg 
				% QString( "SQL Error %1: %2")
					.arg( sqlError.nativeErrorCode())
					.arg( sqlError.text()));
		}
	}
	return query;
}

QSqlQuery Executor::execute( ObjectContext& context, const Model& model,
		const QString& stmt, const QObject* source, const QString& errorMsg) const
{
	// 1. Prepare statement.
	QSqlQuery query( QSqlDatabase::database( m_connName));
	query.prepare( stmt);

	// 2. Bind values
	bindNoMappingColumns( model, query, source);
	bindMappingOneToMany( model, query, context);

	// 3. Execute
	return execute( query, errorMsg);
}


// Executor logging
// =============================================================================

void Executor::logQuery( QSqlQuery& query) const
{
	const QChar quotationMark('\'');
	const QChar openKey('{');
	const QChar closeKey( '}');

	QString message;
	QTextStream os( &message);
	const QMap<QString, QVariant> boundValues = query.boundValues();

	os << QStringLiteral( "QE Orm ") << endl
		<< QStringLiteral( "   + query: ") << query.executedQuery() << endl
		<< QStringLiteral( "   + parameters: { ");
	for( const QString boundKey : boundValues.keys())
	{
		os << openKey << boundKey << QStringLiteral(", ");

		const QVariant value = boundValues[boundKey];
		const QVariant::Type valueType = value.type();
		switch( valueType )
		{
			case QMetaType::UnknownType:
				os << QStringLiteral("null");
				break;
			case QVariant::Type::BitArray:
				{
					QByteArray buffer;
					QDataStream in( buffer);
					in << value.toBitArray();
					os << quotationMark << buffer.toHex()<< quotationMark;
					break;
				}	
			case QVariant::Type::ByteArray:
				os << quotationMark << value.toByteArray().toHex() << quotationMark;
				break;
			case QVariant::Type::Date:
				os << value.toDate().toString( Qt::ISODate);
				break;
			case QVariant::Type::Time:
				os << value.toTime().toString( Qt::ISODate);
				break;
			case QVariant::Type::DateTime:
				os << value.toDateTime().toString( Qt::ISODate);
				break;
			case QVariant::Type::String:
				os << quotationMark << value.toString() << quotationMark;
				break;
			default:
				os << value.toString();
		}
		os << closeKey;
	}
	os << closeKey << endl;

	qCDebug( lcExecutor) << message;
}

// Free functions
// ============================================================================

void bindNoMappingColumns( const Model& model, QSqlQuery& query, 
		const QObject* source)
{
#if 1 
	for( const auto& colDef: model.entityDefs())
	{
		if( colDef->mappingType() == EntityDef::MappingType::NoMappingType)
		{
			QVariant value = source->property( colDef->propertyName());
			if( colDef->isAutoIncrement() && value.toInt() == 0)
				value = QVariant();
			query.bindValue( (QStringLiteral(":") % colDef->entityName()), value);
		}
	}
#else
	const auto queryParamList = query.boundValues().keys();
	for( const auto& queryParam: queryParamList)
	{
		const auto& eDef = model.findEntityDef( Model::findByEntityName{ queryParam});
		if( eDef)
		{
			QVariant value = source->property( eDef->propertyName());
			if( eDef->isAutoIncrement() && value.toInt() == 0)
				value = QVariant();
			query.bindValue( (QStringLiteral(":") % eDef->entityName()), value);
		}
	}
#endif
}

void bindMappingOneToMany( const Model& model, QSqlQuery& query,  
	const ObjectContext& context )
{
	if( !context.empty())
	{
		QObject *contextTopObject = context.back();
		for( const auto& fkDef : model.referencesManyToOneDefs())
		{
			const auto& foreignKeys = fkDef->relationKey();
			const auto& refKeys = fkDef->reference()->primaryKeyDef();
			for( uint i = 0; i < foreignKeys.size(); ++i) 
			{
				const QVariant value = contextTopObject->property( 
						refKeys[i]->propertyName()); 

				query.bindValue( 
						QString( ":%1").arg( foreignKeys[i]->entityName()),
						value);
			}
		}
	}
}

