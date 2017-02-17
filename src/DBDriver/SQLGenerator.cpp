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
 */

#include "SQLGenerator.hpp"
#include <QTextStream>


using namespace std;

QString SQLGenerator::createTableIfNotExist(const QEOrmModel &model) const
{
	QString sqlCommand;
	QTextStream os( &sqlCommand);
	QStringList sqlColumnDef;

	// Generate SQL for each column.
	for( QString column: model.columnNames())
		sqlColumnDef << generateColumnDefinition( model, column);

	// Generate SQL.
	os << QLatin1Literal( "CREATE TABLE IF NOT EXIST '") 
		<< model.table() 
		<< QLatin1Literal("' (")
		<< sqlColumnDef.join( QLatin1Literal(", "));

	// Generate Primary key.
	if( ! model.primaryKey().isEmpty())
		os << QLatin1Literal( "PRIMARY KEY (") << model.primaryKey().join( QLatin1Char(',')) << QLatin1Char(')');

	os << QLatin1Char(')');

	return sqlCommand;
}

QString SQLGenerator::generateColumnDefinition( const QEOrmModel& model, 
													const QString& column) const
{
	QString sqlColumnDef;
	QTextStream os( &sqlColumnDef);
	
	const QEOrmColumnDef columnDef =  model.columnByName( column);
	if( columnDef.isValid())
	{
		const QLatin1Char quotationMark ('\'');
		const QLatin1Char space(' ');
		const uint maxLength = columnDef.maxLength();
		
		// Name
		os << quotationMark << column << quotationMark << space;
		
		// type 
		os << getDBType( 
			static_cast<QMetaType::Type>( columnDef.propertyType()), 
						columnDef.maxLength());
		if( maxLength > 0 )
			os << QLatin1Char('[') << maxLength << QLatin1Char(']');
		os << space;
	
		// Null
		if( columnDef.isNull() )
			os << QLatin1Literal( "NOT NULL ");
		else
			os << QLatin1Literal( "NULL ");
		
		// Default value
		if( ! columnDef.defaultValue().isNull() )
			os << QLatin1Literal( "DEFAULT " ) << columnDef.defaultValue().toString() << space;
		
		// Auto-increment
		if( columnDef.isAutoIncrement())
			os << QLatin1Literal( "AUTO_INCREMENT ");
	}
	
	return sqlColumnDef;
}

QString SQLGenerator::getDBType(const QMetaType::Type propertyType, const uint size) const
{
	switch( propertyType)
	{
		case QMetaType::Bool:
			return QLatin1Literal( "BOOL");
			
		case QMetaType::QChar:
		case QMetaType::Char:
		case QMetaType::SChar:
		case QMetaType::UChar:
			return QLatin1Literal( "CHAR");
			
		case QMetaType::Short:
		case QMetaType::Int:
			return QLatin1Literal( "INT");
		case QMetaType::UShort:
		case QMetaType::UInt:
			return QLatin1Literal( "INT UNSIGNED");
			
		case QMetaType::Long:
		case QMetaType::LongLong:
			return QLatin1Literal( "BIGINT");
			
		case QMetaType::ULong:
		case QMetaType::ULongLong:
			return QLatin1Literal( "BIGINT UNSIGNED");
			
		case QMetaType::Float:
			return QLatin1Literal( "FLOAT");

		case QMetaType::Double:
			return QLatin1Literal( "DOUBLE");

		case QMetaType::QString:
			if( size == 0)
				return QLatin1Literal( "TEXT");
			else
				return QString( "VARCHAR(%1)").arg( size);
			
		case QMetaType::QByteArray:
			return QLatin1Literal( "BLOB");
			
		case QMetaType::QTime:
			return QLatin1Literal( "TIME");
			
		case QMetaType::QDate:
			return QLatin1Literal( "DATE");
			
		case QMetaType::QDateTime:
			return QLatin1Literal( "DATETIME");
			
		case QMetaType::QJsonValue:
		case QMetaType::QJsonObject:
		case QMetaType::QJsonArray:
		case QMetaType::QJsonDocument:
			return QLatin1Literal( "JSON");
			
		case QMetaType::QUuid:
			return QLatin1Literal( "BINARY(16)");
		default:
			return QLatin1Literal( "INT");
	}
}

