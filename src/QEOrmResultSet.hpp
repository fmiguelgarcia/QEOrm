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
 * 
 * GNU Lesser General Public License Usage
 * Alternatively, this file may be used under the terms of the GNU Lesser
 * General Public License version 3 as published by the Free Software
 * Foundation and appearing in the file LICENSE.LGPL3 included in the
 * packaging of this file. Please review the following information to
 * ensure the GNU Lesser General Public License version 3 requirements
 * will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
 *
 * $QE_END_LICENSE$
 */
#pragma once
#include <helpers/QEOrmLoadHelper.hpp>
#include <QObject>
#include <QMetaObject>
#include <QSqlQuery>
#include <iterator>

/// @brief It is an helper class to iterate over database result sets.
template< class T>
class QEOrmResultSet
{
	public:
		/// @brief ResultSet iterator.
		friend class iterator;
		class iterator : public std::iterator<
							  	std::forward_iterator_tag,
								QObject*,
								int,
								QObject**,
								QObject*>
		{
			public:
				explicit iterator( QEOrmResultSet& rs, int pos)
					: m_rs(rs), position(pos) {}

				iterator& operator ++()
				{
				 	position = ( m_rs.m_query.next() ) ? (position +1) : (-1);
			  		return *this;	  
				}

				bool operator ==( const iterator& other) const
				{ return position == other.position; }

				bool operator !=( const iterator& other) const
				{ return position != other.position; }

				/// @brief It creates an object and loads it using the current
				/// result set position.
				T* operator *() const
				{ 
					T* value = reinterpret_cast<T*>( T::staticMetaObject.newInstance( Q_ARG(QObject*, m_rs.m_parent)));
					if( !value)
						throwErrorOnCreateObject( & T::staticMetaObject);

					if( m_rs.m_query.at() != position )
						m_rs.m_query.seek( position);

					QEOrmLoadHelper::load( value, m_rs.m_query);
					return value; 
				}
			private:
				void throwErrorOnCreateObject( const QMetaObject* mo) const { 
					throw std::runtime_error( 
						QString( "QE ORM cannot create an object of type '%1'" 
							"using the constructor %1:%1( QObject* parent)" )
							.arg( mo->className())
							.toStdString());
				}

			private:
				QEOrmResultSet& m_rs;
				int position;
		};

		/// @brief Create an result set from a SQL query.
		/// @param sql SQL query.
		/// @param parent It will be used as a parent for each created object.
		explicit QEOrmResultSet( QSqlQuery sql, QObject* parent = nullptr) 
			: m_query( sql), m_parent( parent)
		{ m_query.next(); }

		iterator begin() const
		{ return iterator( const_cast<QEOrmResultSet&>(*this), 0); }

		iterator end() const
		{ return iterator( const_cast<QEOrmResultSet&>(*this), -1); }

	protected:
		QSqlQuery m_query;	///< Query.
		QObject* m_parent;	///< Parent used for created objects.
};

