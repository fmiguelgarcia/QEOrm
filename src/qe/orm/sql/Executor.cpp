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

#include "Executor.hpp"
#include "generator/ANSIGenerator.hpp"
#include "generator/SQLiteGenerator.hpp"
#include <qe/entity/Model.hpp>
#include <qe/entity/EntityDef.hpp>
#include <qe/entity/RelationDef.hpp>
#include <qe/common/Exception.hpp>
#include <qe/common/DBConnectionPool.hpp>

#if QT_VERSION < QT_VERSION_CHECK( 5, 4, 0)
#	include <qe/orm/sql/GeneratorRepository.hpp>
#endif

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

Q_LOGGING_CATEGORY( qe::orm::sql::lcExecutor, "com.dmious.qe.orm.sql.executor");

// Class Generator
// ============================================================================

Executor::Executor( const QString& connName)
	: m_connName( connName)
{
	// If connection name is empty, it will use the default connection.
	if( m_connName.isEmpty())
		m_connName = QLatin1Literal( QSqlDatabase::defaultConnection);

	QSqlDatabase db = QSqlDatabase::database( m_connName, false);
#if QT_VERSION < QT_VERSION_CHECK( 5, 4, 0)
	m_dbmsType = qe::orm::sql::driverNameToType( db.driverName());
#else
	QSqlDriver* driver = db.driver();
	if( driver)
		m_dbmsType = static_cast<int>( driver->dbmsType());
	else
		m_dbmsType = QSqlDriver::UnknownDbms;
#endif
}

Executor::~Executor()
{}

int Executor::dbmsType() const noexcept
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
	auto dbLockGuard = DBConnectionPool::instance().connection( m_connName);
	QSqlQuery query( dbLockGuard->database());
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
			qCCritical( lcExecutor, "SQL Error '%s' on '%s'",
				qUtf8Printable( sqlError.text()),
				qUtf8Printable( query.lastQuery()));
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
	auto dbLockGuard = DBConnectionPool::instance().connection( m_connName);
	QSqlQuery query( dbLockGuard->database());
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
					os << quotationMark << buffer.left(32).toHex();
					if( buffer.size() > 32)
						os << "...(size=" << buffer.size() << ')';
					os << quotationMark;
					break;
				}	
			case QVariant::Type::ByteArray:
				{
					const QByteArray data = value.toByteArray();
					os << quotationMark << data.left(32).toHex();
					if( data.size() > 32)
						os << "...(size=" << data.size() << ')'; 
					os << quotationMark;
					break;
				}
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
				{
					const QString data = value.toString();
					os << quotationMark << data.left(32);
				  	if( data.size() > 32)
						os << "...(size=" << data.size() << ')';
					os << quotationMark;
					break;
				}
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
	for( const auto& colDef: model.entityDefs())
	{
		if( colDef.mappedType() == EntityDef::MappedType::NoMappedType)
		{
			QVariant value = source->property( colDef.propertyName());
			if( colDef.isAutoIncrement() && value.toInt() == 0)
				value = QVariant();
			if( colDef.isEnum())
				value = colDef.enumerator()->valueToKey( value.toInt());
			query.bindValue( (QStringLiteral(":") % colDef.entityName()), value);
		}
	}
}

void bindMappingOneToMany( const Model& model, QSqlQuery& query,  
	const ObjectContext& context )
{
	if( !context.empty())
	{
		QObject *contextTopObject = context.back();
		const auto fkDef = model.referenceManyToOne();
		if( fkDef)
		{
			const auto& foreignKeys = fkDef->relationKey();
			const auto& refKeys = fkDef->reference().primaryKeyDef();
			for( uint i = 0; i < foreignKeys.size(); ++i) 
			{
				const QVariant value = contextTopObject->property( 
						refKeys[i].propertyName());

				query.bindValue( 
						QString( ":%1").arg( foreignKeys[i].entityName()),
						value);
			}
		}
	}
}

