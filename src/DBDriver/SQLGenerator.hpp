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
#include <QEOrmModel.hpp>

class SQLGenerator
{
	public:
		virtual QString createTableIfNotExist( const QEOrmModel&  model) const;
		
	private:
		virtual QString generateColumnDefinition( 
			const QEOrmModel& model, 
			const QString& column) const;
		
		virtual QString getDBType( const QMetaType::Type propertyType, const uint size) const;
};
