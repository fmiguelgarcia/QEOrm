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
#include <QEOrmColumnDef.hpp>
#include <QTextStream>

using namespace std;

QString SQLGenerator::createTableIfNotExist(const QEOrmModel &model) const
{
	QString sqlCommand;
	QTextStream os( &sqlCommand);
	QStringList sqlColumnDef;
	QString primaryKeyDef;

	// Generate SQL for each column.
	for( QString column: model.columnNames())
		sqlColumnDef << generateColumnDefinition( model, column);
	primaryKeyDef = generatePrimaryKeyDefinition( model);

	// Generate SQL.
	os << QLatin1Literal( "CREATE TABLE IF NOT EXISTS '") 
		<< model.table() 
		<< QLatin1Literal("' (")
		<< sqlColumnDef.join( QLatin1Literal(", "))
		<< primaryKeyDef
		<< QLatin1Char(')');

	return sqlCommand;
}

QString SQLGenerator::generatePrimaryKeyDefinition(const QEOrmModel &model) const
{
	QString pkDef;
	const vector<QEOrmColumnDef> pk = model.primaryKey();
	if( ! pk.empty())
	{
		QStringList pkColumNames;
		for( const QEOrmColumnDef& colDef: pk)
			pkColumNames << colDef.dbColumnName();
		
		pkDef = QString( ", PRIMARY KEY (%1)").arg( pkColumNames.join( QLatin1Char(',')));
	}
	return pkDef;
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
		const uint maxLength = columnDef.dbMaxLength();
		
		// Name
		os << quotationMark << column << quotationMark << space;
		
		// type 
		os << getDBType( 
			static_cast<QMetaType::Type>( columnDef.propertyType()), 
						columnDef.dbMaxLength()) << space;
	
		// Null
		if( columnDef.isDbNullable() )
			os << QLatin1Literal( "NULL ");
		else
			os << QLatin1Literal( "NOT NULL ");
		
		// Default value
		if( ! columnDef.dbDefaultValue().isNull() )
			os << QLatin1Literal( "DEFAULT " ) << columnDef.dbDefaultValue().toString() << space;
		
		// Auto-increment
		if( columnDef.isDbAutoIncrement())
			os << autoIncrementKeyWord() << space; 
	}
	
	return sqlColumnDef;
}

QString SQLGenerator::autoIncrementKeyWord() const
{ return QLatin1Literal( "AUTO_INCREMENT"); }

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


QString SQLGenerator::generateWhereClause( const QObject* o, const QEOrmModel &model) const 
{
	QStringList pkWhereClause;
	for( QEOrmColumnDef colDef: model.primaryKey())
	{
		const QVariant value = o->property( colDef.propertyName().toLocal8Bit().constData());
		pkWhereClause << QString(" %1 == '%2'")
				.arg( colDef.dbColumnName())
				.arg( value.toString());
	}
	
	return pkWhereClause.join( QLatin1Literal(" AND "));
}

QString SQLGenerator::generateExistsObjectOnDBStmt(const QObject *o, const QEOrmModel &model) const
{
	QString stmt;
	QTextStream os;
	
	os << QLatin1Literal( "SELECT * FROM '") << model.table() << QLatin1Literal("' WHERE ");
	os << generateWhereClause( o, model) << QLatin1Literal( " LIMIT 1");
	
	return stmt;
}

QString SQLGenerator::generateInsertObjectStmt( const QObject *o, const QEOrmModel &model) const 
{

	QStringList values;
	const QStringList columnNames = model.columnNames();
	for( const QString& colName: columnNames)
	{
		const QEOrmColumnDef colDef = model.columnByName( colName);
		const QVariant value = o->property( colDef.propertyName().toLocal8Bit().constData());
		values << value.toString();
	}
	
	QString stmt;
	QTextStream os;
	os << QLatin1Literal( "INSERT INTO '") << model.table() 
		<< QLatin1Literal("' (") << columnNames.join( QLatin1Literal(", ")) 
		<< QLatin1Literal(") VALUES (") << values.join( QLatin1Literal(", ")) 
		<< QLatin1Literal(")");

	return stmt;
}

QString SQLGenerator::generateUpdateObjectStmt( const QObject *o, const QEOrmModel& model) const
{

	QStringList setExpList;
	for( const QEOrmColumnDef& col: model.noPrimaryKey())
	{
		const QVariant value = o->property( col.propertyName().toLocal8Bit().constData());
		setExpList << QString("%1 = %2")
			.arg( col.dbColumnName())
			.arg( value.toString());
	}
	
	QString stmt;
	QTextStream os;
	os << QLatin1Literal( "UPDATE '") << model.table() 
		<< QLatin1Literal( "' SET ") << setExpList.join( QLatin1Literal(", "))
		<< QLatin1Literal( " WHERE ") << generateWhereClause( o, model);

	return stmt;
}