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

#include "ContactInfo.hpp"

namespace {
    const static int ContactInfoId = qRegisterMetaType<ContactInfo*>();
}

ContactInfo::ContactInfo( QObject* parent, int id, QString email, QString phone)
	: QObject(parent), id(id), email( email), phone( phone)
{}
		
ContactInfo::ContactInfo( const ContactInfo& other) noexcept
	: QObject( nullptr),
	id( other.id),
	email( other.email),
	phone( other.phone)
{}

ContactInfo& ContactInfo::operator=( const ContactInfo& other) noexcept
{
	id = other.id;
	email = other.email;
	phone = other.phone;
	return *this;
}
		
bool ContactInfo::operator==(const ContactInfo &other) const
{
	return email == other.email && 
		phone == other.phone;
}
