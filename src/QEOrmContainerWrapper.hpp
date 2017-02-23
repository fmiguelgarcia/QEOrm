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
#include <QVariant>

QE_BEGIN_NAMESPACE

#define QE_ORM_MAP_ONE_TO_MANY( property, member ) \
	Q_PROPERTY( QVariantList property READ member##CW__ WRITE member##CW__ ) \
	Q_CLASSINFO( "property", "@QE.ORM.MAPPING.TYPE=OneToMany @QE.ORM.MAPPING.ENTITY=__typeof__(property)") \
	QEOrmContainerWrapper< decltype( member ) > member##CW__ { member };

template< class C>
class QEOrmContainerWrapper
{
	private:
		C& m_container;
		using CValueType = typename C::value_type;
		
	public: 
		QEOrmContainerWrapper(C& container) : m_container( container)
		{}

		// Read function
		QVariantList operator()() const
		{
			QVariantList list;
			for( CValueType& item : m_container)
				list << QVariant::fromValue( std::addressof(item));
			return list;
		}

		// Write function
		void operator()( const QVariantList& list)
		{
			using CItem = typename C::value_type;
			m_container.clear();
			for( const QVariant &item : list)
			{
				CItem * nativeItem = item.value<CItem*>();
				m_container.push_back( *nativeItem);
			}
		}

};
QE_END_NAMESPACE
