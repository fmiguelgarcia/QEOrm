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
#include "QEOrmTest.hpp"
#include "AnnotateClassOne.hpp"
#include <QEOrm.hpp>
#include <QSqlDatabase>
#include <QtTest>

using namespace std;
QTEST_MAIN(QEOrmTest);

void QEOrmTest::initTestCase()
{
	// Called before the first testfunction is executed
	// Settup database
	QSqlDatabase db = QSqlDatabase::addDatabase( "QSQLITE");
//	db.setDatabaseName(":memory:");
	db.setDatabaseName("/tmp/QEOrmTest.db");
	QVERIFY( db.open() );
}

void QEOrmTest::cleanupTestCase()
{
	// Called after the last testfunction was executed
}

void QEOrmTest::init()
{
	// Called before each testfunction is executed
}

void QEOrmTest::cleanup()
{
	// Called after every testfunction
}

void QEOrmTest::checkTableCreation()
{
	AnnotateClassOne obj1, obj2;
	obj1.m_id = 0;
	obj1.m_user = "Miguel";
	obj1.m_begin = QDateTime::currentDateTime();
	obj1.m_end = obj1.m_begin.addSecs(660);

	FKClass fk1,fk2;
	fk1.value = 3.14;
	fk1.description = QLatin1Literal( "This is PI");
	fk2.value = 2.71828;
	fk2.description = QLatin1Literal( "This is e");
	vector< FKClass > fkList = { fk1, fk2};
#if 0
	QVariantList fkVarList = { QVariant::fromValue(fk1), QVariant::fromValue(fk2) };

	obj1.setProperty( "fkClass", fkVarList);
	QVERIFY( fkList == obj1.fkClass());
#else
	obj1.setFkClass( {fk1,fk2});

	QEOrm::instance().save( &obj1);
	QEOrm::instance().load( { obj1.m_id}, &obj2);
	
	QVERIFY( obj1 == obj2);
#endif
}


