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
#include "entity/book.hpp"
#include <qe/orm/QEOrm.hpp>
#include <qe/common/Exception.hpp>

#include <QSqlDatabase>
#include <QCryptographicHash>
#include <QtTest>

using namespace qe::orm;
using namespace qe::common;
using namespace std;

QTEST_MAIN(QEOrmTest);

QEOrmTest::QEOrmTest( QObject* parent)
	: QObject( parent)
{
	// Called before the first testfunction is executed
	// Settup database
	QSqlDatabase db = QSqlDatabase::addDatabase( "QSQLITE");

	//	db.setDatabaseName(":memory:");
	db.setDatabaseName("/tmp/QEOrmTest.db");
	QVERIFY( db.open() );
}

void QEOrmTest::checkSaveAutoIncrement()
{
	Book book;
	book.title = "Effective Modern C++";
	book.author = "Scott Meyers";
	book.pages = 303;
  	book.binSignature = QCryptographicHash::hash( book.title.toUtf8(),
		QCryptographicHash::Sha256);	
	
	QEOrm::instance().save( &book);
	QVERIFY( book.id != 0);
}

void QEOrmTest::checkSaveReferences()
{
	Book book;
	book.title = "Effective Modern C++";
	book.author = "Scott Meyers";
	book.pages = 303;
  	book.binSignature = QCryptographicHash::hash( book.title.toUtf8(),
		QCryptographicHash::Sha256);

	// Save book to get its id.
	QEOrm::instance().save( &book);
	QVERIFY( book.id != 0);

	// Chapters don't use autoincrement id, so we generate them from book.id
	Chapter ch1( nullptr, "Deducing Types");
	ch1.text = "- During template type deduction, arguments that are references "
		"are treated as non-references, i.e., their reference-ness is ignored."
		"\n - When deducing types for universal reference parameters, lvalue "
		"arguments get special treatment.";
	ch1.id = 1 + book.id * 1000;
	ch1.setObjectName( "Chapter 1");
	
	Chapter ch2( nullptr, "Understand auto type deduction");
	ch2.text = " - auto type deduction is usually the same as template type "
		"deduction, but auto type  deduction  assumes  that  a  braced  "
		"initializer  represents  a  std::initializer_list, and template type"
		"deduction doesn’t.\n -  auto in  a  function  return  type  or  a "
		"lambda  parameter  implies  template  type deduction, not auto type "
		"deduction.";
	ch2.id = 2 + book.id * 1000;

	// Update book and save.
	book.chapters = { ch1, ch2};
	QEOrm::instance().save( &book);
	
	Book loadedBook;
	QEOrm::instance().load( QVariantList{ book.id }, &loadedBook);
	
	QVERIFY( book == loadedBook);
}


