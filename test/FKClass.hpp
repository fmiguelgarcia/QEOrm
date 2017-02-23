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

class FKClass : public QObject
{
	Q_OBJECT
	Q_PROPERTY( int id MEMBER id)
	Q_PROPERTY( double value MEMBER value)
	Q_PROPERTY( QString description MEMBER description)
	
	Q_CLASSINFO( "id", "@QE.ORM.AUTO_INCREMENT=true")
	Q_CLASSINFO( "value", "@QE.ORM.NULL=false")

	public:
		explicit FKClass( QObject* parent = nullptr);
		FKClass( const FKClass& other) noexcept;
		FKClass& operator= ( const FKClass& other) noexcept;
	
	public:
		int id;
		double value;
		QString description;
};
Q_DECLARE_METATYPE( FKClass*);
