/*
 * Copyright (C) 2017 francisco miguel garcia rodriguez  
 * Contact: http://www.dmious.com/qe/licensing/
 *
 * This file is part of the QE Common module of the QE Toolkit.
 *
 * $QE_BEGIN_LICENSE:LGPL21$
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
 * General Public License version 3 as published by the Free
 * Software Foundation and appearing in the file LICENSE.LGPLv3 
 * included in the packaging of this file. Please review the
 * following information to ensure the GNU Lesser General Public License
 * requirements will be met: https://www.gnu.org/licenses/lgpl.html and
 * http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
 *
 * $QE_END_LICENSE$
 */

#pragma once
#include <QECommon/QEGlobal.hpp>
#include <QVariant>

QE_BEGIN_NAMESPACE

#define QE_ORM_MAP_ONE_TO_MANY( property, member ) \
	Q_PROPERTY( QVariantList property READ member##CW__ WRITE member##CW__ ) \
	Q_CLASSINFO( "property", "@QE.ORM.MAPPING.TYPE=OneToMany @QE.ORM.MAPPING.ENTITY=__typeof__(property)") \
	QEOrmContainerWrapper< decltype( member ) > member##CW__ { member };

/// @brief This wrapper help you to expose relation properties using
/// QVariantList instead of a native container.
template< class C>
class QEOrmContainerWrapper
{
	private:
		C& m_container;
		using CValueType = typename C::value_type;
		
	public: 
		QEOrmContainerWrapper(C& container) : m_container( container)
		{}

		/// @brief This read function loads all elements of the native container
		/// as a QVariantList.
		QVariantList operator()() const
		{
			QVariantList list;
			for( CValueType& item : m_container)
				list << QVariant::fromValue( std::addressof(item));
			return list;
		}

		/// @brief This write function stores all elements from QVariantList to
		/// the native container. 
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
