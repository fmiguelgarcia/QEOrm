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

#include "QEOrmTest.hpp"
#include "AnnotateClassOne.hpp"
#include <QEOrm.hpp>
#include <QSqlDatabase>
#include <QtTest>

QTEST_MAIN(QEOrmTest);

void QEOrmTest::initTestCase()
{
	// Called before the first testfunction is executed
	// Settup database
	QSqlDatabase db = QSqlDatabase::addDatabase( "QSQLITE");
	db.setDatabaseName(":memory:");
//	db.setDatabaseName("/tmp/QEOrmTest.db");
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
	obj1.m_id = 1;
	obj1.m_user = "Miguel";
	obj1.m_begin = QDateTime::currentDateTime();
    obj1.m_end = obj1.m_begin.addSecs(660);

#if 0
    FKClass x;
    const QMetaObject *xMo = x.metaObject();
    QByteArray className = xMo->className();
    int fkClassId = QMetaType::type( className);
    QMetaType fkClassMT( fkClassId);
    const QMetaObject* fkClassMO = fkClassMT.metaObject();

    /*char* rawAllocatedSpace = new char[ fkClassMT.sizeOf()];
    QObject * obj = reinterpret_cast<QObject*>(fkClassMT.construct( rawAllocatedSpace));
    const QMetaObject* mo = obj->metaObject();*/

    className = QMetaObject::normalizedType( QString("%1 *").arg(xMo->className()).toLocal8Bit().constData());
    fkClassId = QMetaType::type( className);
    QMetaType fkClassPtrMT( fkClassId);
    const QMetaObject* fkClassPtrMO = fkClassPtrMT.metaObject();

    QSharedPointer<FKClass> spx( new FKClass());
    const QMetaObject *spxMo = spx->metaObject();
    QByteArray spxClassName = spxMo->className();
    int spxType = QMetaType::type( spxClassName);
    QMetaType spxMT( spxType);
    const QMetaObject* spxMTMO = spxMT.metaObject();
#endif

	QEOrm::instance().save( &obj1);
	QEOrm::instance().load( {1}, &obj2);
	
	QVERIFY( obj1 == obj2);
}


