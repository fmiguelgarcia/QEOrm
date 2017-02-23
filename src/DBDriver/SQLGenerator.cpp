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
#include "SQLGenerator.hpp"
#include <QEOrmColumnDef.hpp>
#include <QTextStream>
#include <QDateTime>
#include <algorithm>

using namespace std;
namespace {

	/// @brief It returns a list of strings contains the DB column names.
	template< class C>
	QStringList columnDbNames( const C& container)
	{
		QStringList colDbNames;
		for( const QEOrmColumnDef& colDef: container)
			colDbNames << colDef.dbColumnName();
		return colDbNames;	
	}
}

QString SQLGenerator::createTableIfNotExist(const QEOrmModel &model) const
{
	QString sqlCommand;
	QTextStream os( &sqlCommand);
	QStringList sqlColumnDef;

	// Generate SQL for each column.
	for( QEOrmColumnDef columnDef: model.columns())
	{
		if( columnDef.mappingType() == QEOrmColumnDef::MappingType::NoMappingType)
			sqlColumnDef << generateColumnDefinition( columnDef);
	}

	QString primaryKeyDef = generatePrimaryKeyDefinition( model);
	QString foreignKeyDef = generateForeignKeyDefinition( model);

	// Generate SQL.
	os << QLatin1Literal( "CREATE TABLE IF NOT EXISTS '") 
		<< model.table() << QLatin1Literal("' (")
		<< sqlColumnDef.join( QLatin1Literal(", "))
		<< primaryKeyDef << foreignKeyDef << QLatin1Char(')');

	return sqlCommand;
}

QStringList SQLGenerator::createTablesIfNotExist(const QEOrmModel &model) const
{
	QStringList sqlCommands;
	
	sqlCommands << createTableIfNotExist(model);
	for( QEOrmColumnDef columnDef: model.columns())
	{
		if( columnDef.mappingType() != QEOrmColumnDef::MappingType::NoMappingType)
		{
			/// @todo Generate table and add column if
			QEOrmModel mapModel( columnDef.mappingEntity());
			mapModel.addRefToOne( model);
			sqlCommands << createTablesIfNotExist( mapModel);
		}
	}
	
	return sqlCommands;
}

QString SQLGenerator::generatePrimaryKeyDefinition(const QEOrmModel &model) const
{
	QString pkDef;
	const vector<QEOrmColumnDef> pk = model.primaryKey();
	if( ! pk.empty())
	{
		const QStringList pkColumNames = columnDbNames(pk);
		pkDef = QString( ", PRIMARY KEY (%1)").arg( pkColumNames.join( QLatin1Char(',')));
	}
	return pkDef;
}

QString SQLGenerator::generateForeignKeyDefinition( const QEOrmModel& model) const
{
	QString fkDef;
	QTextStream os( &fkDef);
	const QChar comma = QLatin1Char(',');

	for( const QEOrmForeignDef& fkDef : model.referencesToOne())
	{
		const QEOrmModel reference = fkDef.reference();
		const QStringList fkColNames = columnDbNames( fkDef.foreignKeys());
		const QStringList refColNames = columnDbNames( fkDef.referenceKeys());

		os << QLatin1Literal( ", FOREIGN KEY (") << fkColNames.join( comma)  
			<< QLatin1Literal( ") REFERENCES ") << reference.table() << QLatin1Char('(')
			<< refColNames.join( comma) << QLatin1Char(')')
			<< QLatin1Literal( " ON DELETE CASCADE")
			<< QLatin1Literal( " ON UPDATE CASCADE");
	}
	return fkDef;
}

QString SQLGenerator::generateColumnDefinition( const QEOrmColumnDef column) const
{
	QString sqlColumnDef;
	QTextStream os( &sqlColumnDef);
	
	const QLatin1Char quotationMark ('\'');
	const QLatin1Char space(' ');
		
	// Name
	os << quotationMark << column.dbColumnName() << quotationMark << space;
		
	// type 
	os << getDBType( 
		static_cast<QMetaType::Type>( column.propertyType()), 
						column.dbMaxLength()) << space;
	
	// Null
	if( column.isDbNullable() )
		os << QLatin1Literal( "NULL ");
	else
		os << QLatin1Literal( "NOT NULL ");
		
	// Default value
	if( ! column.dbDefaultValue().isNull() )
		os << QLatin1Literal( "DEFAULT " ) << column.dbDefaultValue().toString() << space;
		
	// Auto-increment
	if( column.isDbAutoIncrement())
		os << autoIncrementKeyWord() << space; 
	
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


QString SQLGenerator::generateWhereClause( const QEOrmModel &model) const 
{
	QStringList pkWhereClause;
	vector<QEOrmColumnDef> pkColumns = model.primaryKey();

	for( const QEOrmColumnDef& colDef : pkColumns) 
		pkWhereClause << QString(" %1 == :%2")
			.arg( colDef.dbColumnName())
			.arg( colDef.dbColumnName());
	
	return pkWhereClause.join( QLatin1Literal(" AND "));
}

/*
QString SQLGenerator::generateWhereClause( const QObject* o, const QEOrmModel &model) const 
{
	QStringList pkWhereClause;
	for( QEOrmColumnDef colDef: model.primaryKey())
	{
		const QVariant value = o->property( colDef.propertyName().constData());
		pkWhereClause << QString(" %1 == %2")
				.arg( colDef.dbColumnName())
				.arg( variantToSQL( value, colDef.propertyType()));
	}
	
	return pkWhereClause.join( QLatin1Literal(" AND "));
}*/

QString SQLGenerator::generateExistsObjectOnDBStmt(const QObject *o, const QEOrmModel &model) const
{
	QString stmt;
	QTextStream os( &stmt);
	
	os << QLatin1Literal( "SELECT * FROM '") << model.table() << QLatin1Literal("' WHERE ");
	os << generateWhereClause( model) << QLatin1Literal( " LIMIT 1");
	
	return stmt;
}

QString SQLGenerator::generateInsertObjectStmt( const QObject *o, const QEOrmModel &model) const 
{

	QStringList values;
	QStringList columnNames;

	for( const QEOrmColumnDef colDef: model.columns())
	{
		if( colDef.mappingType() == QEOrmColumnDef::MappingType::NoMappingType)
		{
			columnNames << colDef.dbColumnName();
			values << QString( ":%1").arg( colDef.dbColumnName());
		}
	}
	
	QString stmt;
	QTextStream os( &stmt);
	os << QLatin1Literal( "INSERT INTO '") << model.table() 
		<< QLatin1Literal("' (") << columnNames.join( QLatin1Literal(", ")) 
		<< QLatin1Literal(") VALUES (") << values.join( QLatin1Literal(", ")) 
		<< QLatin1Literal(")");

	return stmt;
}

QString SQLGenerator::generateUpdateObjectStmt( const QObject *o, const QEOrmModel& model) const
{

	QStringList setExpList;
	for( const QEOrmColumnDef& col: model.columns())
	{
		if( !col.isPartOfPrimaryKey())
		{
			// const QVariant value = o->property( col.propertyName().constData());
			setExpList << QString("%1 = :%2")
				.arg( col.dbColumnName())
				.arg( col.dbColumnName());
		}
	}
	
	QString stmt;
	QTextStream os( &stmt);
	os << QLatin1Literal( "UPDATE '") << model.table() 
		<< QLatin1Literal( "' SET ") << setExpList.join( QLatin1Literal(", "))
		<< QLatin1Literal( " WHERE ") << generateWhereClause( model);

	return stmt;
}

QString SQLGenerator::generateLoadObjectFromDBStmt(const QVariantList& pk, const QEOrmModel &model) const
{
	QStringList columns;
	for( const QEOrmColumnDef& colDef : model.columns())
		if( colDef.mappingType() == QEOrmColumnDef::MappingType::NoMappingType)
			columns << colDef.dbColumnName();

	QString stmt;
	QTextStream os( &stmt);
	os << QLatin1Literal(" SELECT ") << columns.join( ", ")
		<< QLatin1Literal( " FROM '") << model.table() << QLatin1Char('\'')
		<< QLatin1Literal( " WHERE ") << generateWhereClause( model);
	return stmt;
}
