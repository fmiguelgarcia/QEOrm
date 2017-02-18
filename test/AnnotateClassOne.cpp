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

#include "AnnotateClassOne.hpp"

AnnotateClassOne::AnnotateClassOne(QObject *parent)
	: QObject( parent)
{ }

bool AnnotateClassOne::operator==(const AnnotateClassOne &other) const
{
	return m_id == other.m_id
		&& m_user == other.m_user
		&& m_begin == other.m_begin
		&& m_end == other.m_end;
}
