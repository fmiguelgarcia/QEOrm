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

#include "SQLiteGenerator.hpp"
#include <qe/entity/Model.hpp>
#include <qe/entity/EntityDef.hpp>

using namespace qe::entity;
using namespace qe::orm::sql;

QString SQLiteGenerator::autoIncrementKeyWord() const
{ return QStringLiteral("AUTOINCREMENT"); }

QString SQLiteGenerator::makeColumnDefinition( 
	const Model& model, const EntityDef& column) const
{
	QString sqlColumnDef;
	if( !column.isAutoIncrement())
			sqlColumnDef = ANSIGenerator::makeColumnDefinition( model, column);
	else
		sqlColumnDef = QString( "'%1' INTEGER PRIMARY KEY %2 ")
			.arg( column.entityName())
			.arg( autoIncrementKeyWord());

	return sqlColumnDef;
}

QString SQLiteGenerator::makePrimaryKeyDefinition( const Model &model) const
{
	QString sqlStmt;
	
	const auto autoIncrementDef = model.findEntityDef( Model::findByAutoIncrement{});
	if( ! autoIncrementDef) 
		sqlStmt = ANSIGenerator::makePrimaryKeyDefinition( model);

	return sqlStmt;
}

QString SQLiteGenerator::databaseType( 
	const EntityDef& eDef) const 
{
	if( eDef.isEnum())
		return databaseEnumerationType( eDef);

	QString dbType;
	const int size = eDef.maxLength();
	switch( eDef.propertyType())
	{
		case QMetaType::Bool:
			dbType = QStringLiteral( "BOOLEAN");
			break;
		case QMetaType::QChar:
		case QMetaType::Char:
		case QMetaType::SChar:
		case QMetaType::UChar:
			dbType = QStringLiteral( "CHAR");
			break;
		case QMetaType::UShort:
		case QMetaType::UInt:			
		case QMetaType::ULong:
		case QMetaType::ULongLong:
			dbType = QStringLiteral( "INTEGER UNSIGNED");
			break;
		case QMetaType::Float:
		case QMetaType::Double:
			dbType = QStringLiteral( "REAL");
			break;
		case QMetaType::QString:
			if( size == 0)
				dbType = QStringLiteral( "TEXT");
			else
				dbType = QString( "VARCHAR(%1)").arg( size);
			break;
		case QMetaType::QByteArray:
			if( size == 0)
				dbType = QStringLiteral( "BLOB");
			else
				dbType = QString( "VARBINARY(%1)").arg( size);
			break;	
		case QMetaType::QTime:
			dbType = QStringLiteral( "TIME");
			break;	
		case QMetaType::QDate:
			dbType = QStringLiteral( "DATE");
			break;
		case QMetaType::QDateTime:
			dbType = QStringLiteral( "DATETIME");
			break;	
		case QMetaType::QJsonValue:
		case QMetaType::QJsonObject:
		case QMetaType::QJsonArray:
		case QMetaType::QJsonDocument:
		case QMetaType::QUuid:
			dbType = QStringLiteral( "TEXT");
			break;
		case QMetaType::Short:
		case QMetaType::Int:
		case QMetaType::Long:
		case QMetaType::LongLong:
		default:
			dbType = QStringLiteral( "INTEGER");
	}
	return dbType;
}

QString SQLiteGenerator::databaseEnumerationType( 
	const EntityDef& ) const 
{
	return QStringLiteral( "TEXT");
}

