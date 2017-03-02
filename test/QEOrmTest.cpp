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
#include "Person.hpp"
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
	Person p1, p2;
	p1.name = "Miguel";
	p1.begin = QDateTime::currentDateTime();

	ContactInfo ci1, ci2;
	ci1.email = "info@dmious.com";
	ci1.phone = "091";
	ci2.email = "google@google.com";
	ci2.phone = "555";

	p1.setContactInfoList( {ci1, ci2});

	QEOrm::instance().save( &p1);
	QEOrm::instance().load( { p1.id}, &p2);
	
	QVERIFY( p1 == p2);
}

void QEOrmTest::findTest()
{
	int count = 0;
	Person p1;
	p1.name = "Miguel";
	p1.begin = QDateTime::currentDateTime();
	p1.end = p1.begin.addMSecs( 1000 * 60 );

	QEOrm::instance().save( &p1);

	auto rs = QEOrm::instance().findEqual<Person>( {{QString("name"), "Miguel"}});
	auto itr = begin( rs);
	while( itr != end(rs))
	{
		Person *pFound = *itr;
		if( pFound)
		{
			QVERIFY( pFound->name == "Miguel");
			++count;	
		}
		++itr;
	}

	QVERIFY( count > 0);
}

void QEOrmTest::updateTest()
{
	Person p1;
	p1.name = "UpdateTestUser";
	p1.begin = QDateTime::currentDateTime();

	ContactInfo ci1, ci2;
	ci1.email = "info@dmious.com";
	ci1.phone = "091";
	ci2.email = "google@google.com";
	ci2.phone = "555";
	p1.setContactInfoList( {ci1, ci2});

	QEOrm::instance().save( &p1);

	auto ciList = p1.contactInfoList();
	QVERIFY( ! ciList.empty());

	ciList.push_back( ContactInfo( nullptr, 0, "xxx@google.com", "666")); 
	p1.setContactInfoList( ciList);

	QEOrm::instance().save( &p1);
}


