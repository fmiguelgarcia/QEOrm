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
 * $QE_END_LICENSE$
 *
 */

#include "MySQLGenerator.hpp"
#include <qe/entity/EntityDef.hpp>
#include <limits>

using namespace qe::orm::sql;
using namespace std;

MySQLGenerator::~MySQLGenerator()
{}

QString qe::orm::sql::MySQLGenerator::databaseType(
	const entity::EntityDef& eDef) const
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
			dbType = QStringLiteral( "INTEGER UNSIGNED");
		case QMetaType::UInt:
		case QMetaType::ULong:
		case QMetaType::ULongLong:
			dbType = QStringLiteral( "BIGINT UNSIGNED");
			break;
		case QMetaType::Float:
			dbType = QStringLiteral( "FLOAT");
		case QMetaType::Double:
			dbType = QStringLiteral( "REAL");
			break;
		case QMetaType::QString:
			if( size == 0 )
				dbType = QStringLiteral( "TEXT");
			else if( size < (1 << 8)) 		// 2^8 -1
				dbType = QString( "VARCHAR(%1)").arg( size);
			else if( size < (1 << 16))		// 2^16 -1
				dbType = QStringLiteral( "TEXT");
			else if( size < (1 << 24))		// 2^24 -1
				dbType = QStringLiteral( "MEDIUMTEXT");
			else
				dbType = QString( "LONGTEXT");
			break;
		case QMetaType::QByteArray:
			if( size == 0 )
				dbType = QStringLiteral( "BLOB");
			else if( size < (1 << 8)) 		// 2^8 -1
				dbType = QString( "VARBINARY(%1)").arg( size);
			else if( size < (1 << 16))		// 2^16 -1
				dbType = QStringLiteral( "BLOB");
			else if( size < (1 << 24))		// 2^24 -1
				dbType = QStringLiteral( "MEDIUMBLOB");
			else
				dbType = QString( "LONGBLOB");
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
			dbType = QStringLiteral( "JSON");
			break;
		case QMetaType::QUuid:
			dbType = QStringLiteral( "CHAR(38)");
			break;
		case QMetaType::Int:
		case QMetaType::Long:
		case QMetaType::LongLong:
			dbType = QStringLiteral( "BIGINT");
			break;
		case QMetaType::Short:
		default:
			dbType = QStringLiteral( "INTEGER");
	}
	return dbType;
}
