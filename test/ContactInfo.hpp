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

#include <QObject>

class ContactInfo: public QObject
{
	Q_OBJECT
	Q_PROPERTY( int id MEMBER id)
	Q_PROPERTY( QString email MEMBER email)
	Q_PROPERTY( QString phone MEMBER phone)
	
	Q_CLASSINFO( "id", "@QEOrm.isAutoIncrement=true")
	Q_CLASSINFO( "email", "@QEOrm.maxLenght=512")
	Q_CLASSINFO( "phone", "@QEOrm.maxLenght=32")
	
	public:
		explicit ContactInfo( QObject* parent = nullptr);
		ContactInfo( const ContactInfo& other) noexcept;
		ContactInfo& operator= ( const ContactInfo& other) noexcept;

		bool operator==( const ContactInfo& other) const ;
	
	public:
		int id = 0;
		QString email;
		QString phone;
};
Q_DECLARE_METATYPE( ContactInfo*);
Q_DECLARE_METATYPE( ContactInfo);
