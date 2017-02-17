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
#include <QECommon/QEGlobal.hpp>
#include <QMetaType>
#include <QString>
#include <QVariant>

QE_BEGIN_NAMESPACE 

class QEOrmModel;
class QEOrmColumnDef
{
	public:
		QEOrmColumnDef();
		QEOrmColumnDef( const QString property, 
						const int type,
						const QEOrmModel* model);
		QEOrmColumnDef( const QEOrmColumnDef &other);
		
		bool isValid() const noexcept;
	
		QString propertyName() const noexcept;
		QString columnName() const noexcept;
		uint maxLength() const noexcept;
		bool isAutoIncrement() const noexcept;
		bool isNull() const noexcept;
		bool isUnique() const noexcept;
		int propertyType() const noexcept;
		QVariant defaultValue() const noexcept;
		
	private:
		QString m_propertyName;
		QString m_columnName;
		QVariant m_defaultValue;
		int m_propertyType;
		uint m_maxLength;
		bool m_isAutoIncrement;
		bool m_isNull;
		bool m_isUnique;
		
};

QE_END_NAMESPACE
