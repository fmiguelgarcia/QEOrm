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

#pragma once
#include "ContactInfo.hpp"
#include <QEOrmContainerWrapper.hpp>
#include <QObject>
#include <QString>
#include <QDateTime>
#include <QVariant>

class Person
	: public QObject
{
	Q_OBJECT
	Q_PROPERTY( int id MEMBER id)
	Q_PROPERTY( QString name MEMBER name)
	Q_PROPERTY( QDateTime begin MEMBER begin )
	Q_PROPERTY( QDateTime end MEMBER end)
	
	Q_CLASSINFO( "id", "@QEOrm.isAutoIncrement=true")
	Q_CLASSINFO( "name", "@QEOrm.isNull=false @QEOrm.maxLength=256")
	Q_CLASSINFO( "end", "@QEOrm.isNull=true")
	
	Q_PROPERTY( QVariantList contactInfo READ m_ContactWrapper WRITE m_ContactWrapper)
	Q_CLASSINFO( "contactInfo", "@QEOrm.mapping.type=OneToMany @QEOrm.mapping.entity=ContactInfo")

	public:
		explicit Person( QObject* parent = nullptr);
		bool operator == ( const Person& other) const;
		

		std::vector<ContactInfo> contactInfoList() const;
		void setContactInfoList( const std::vector<ContactInfo>& );

	public:
		int id = 0;
		QString name;
		QDateTime begin;
		QDateTime end;

		
		std::vector<ContactInfo> m_contactInfoList;
		QEOrmContainerWrapper< decltype(m_contactInfoList) > m_ContactWrapper{m_contactInfoList};
		
		//QE_ORM_MAP_ONE_TO_MANY( fk2, m_fkClass2)
};
