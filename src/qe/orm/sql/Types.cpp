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
#include "Types.hpp"

#if QT_VERSION < QT_VERSION_CHECK(5,4,0)
#include <iterator>

using namespace qe::orm::sql;
using namespace std;

QSqlDriver qe::orm::sql::driverNameToType( const QString& name)
{
	static std::array< QString, 9> driverNames = {
		QString(), 						// UnknownDbms = 0,
		QStringLiteral("QODBC"), 	// MSSqlServer,
		QStringLiteral("QMYSQL"), 	// MySqlServer,
		QStringLiteral("QPSQL"), 	// PostgreSQL,
		QStringLiteral("QOCI"), 	// Oracle,
		QStringLiteral("QTDS"), 	// Sybase = 5,
		QStringLiteral("QSQLITE"),	// SQLite,
		QStringLiteral("QIBASE"),	// Interbase,
		QStringLiteral("QDB2"),		//  DB2
	};

	auto itr = find( begin( driverNames), end( driverNames), name);
	if( itr == end( driverNames))
		itr = begin( driverNames);

	return distance( begin( driverNames), itr);
}

#endif

