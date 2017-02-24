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

#include "Person.hpp"
using namespace std;

Person::Person(QObject *parent)
	: QObject( parent)
{}

bool Person::operator ==(const Person &other) const
{
	return id == other.id
		&& name == other.name
		&& begin == other.begin
		&& end == other.end
		&& m_contactInfoList == other.m_contactInfoList;
}


vector<ContactInfo> Person::contactInfoList() const
{ return m_contactInfoList;}

void Person::setContactInfoList( const std::vector<ContactInfo>& list )
{ m_contactInfoList = list; }
