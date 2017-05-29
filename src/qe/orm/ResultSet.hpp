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
#include <qe/orm/Global.hpp>
#include <QObject>
#include <QMetaObject>
#include <QSqlQuery>
#include <iterator>

namespace qe { namespace orm {
	class ResultSetIteratorBasePrivate;
	template< class T> class ResultSet;

	class QEORM_EXPORT ResultSetIteratorBase
		: public std::iterator< std::forward_iterator_tag, QObject*, int,
			QObject**, QObject*>
	{
		public:
			virtual ~ResultSetIteratorBase();

		protected:
			QObject* createInstance( const QMetaObject* mo, QObject* parent) const;
			void loadFromQuery( QObject *o, QSqlQuery& query) const;

			ResultSetIteratorBasePrivate *d_ptr;

		private:
			Q_DECLARE_PRIVATE( ResultSetIteratorBase);
	};

	template< class T, typename ...Args >
	class ResultSetValueBuilder {
		public:
			inline static T createInstance( Args&&... args)
			{
				T value( std::forward<Args...>(args...));
				return value;
			}
	};

	
	template< class T>
	class ResultSetIterator
		: public ResultSetIteratorBase
	{
		public:
			explicit ResultSetIterator( ResultSet<T>& rs, int pos) noexcept
				: m_rs(rs), m_position(pos)
			{}

			ResultSetIterator( const ResultSetIterator& other) noexcept
				: m_rs( other.m_rs), m_position( other.m_position)
			{}

			ResultSetIterator& operator ++()
			{
				m_rs.m_query.next();
				m_position = m_rs.m_query.at();
				return *this;
			}

			inline bool operator ==( const ResultSetIterator& other) const noexcept
			{ return m_position == other.m_position; }

			inline bool operator !=( const ResultSetIterator& other) const noexcept
			{ return m_position != other.m_position;}

#if 0
			/// @brief It creates an object and loads it using the current
			/// result set position.
			T* operator *() const
			{ 
				T* value = reinterpret_cast<T*>( 
					createInstance( 
						std::addressof( T::staticMetaObject),
						m_rs.m_parent)); 
			
				loadFromQuery( value, m_rs.m_query);
				return value; 
			}
#else
			inline void to( T& target) const
			{ loadFromQuery( std::addressof(target), m_rs.m_query); }

			T operator* () const
			{
				T value = ResultSetValueBuilder<T>::createInstance();
				loadFromQuery( value, m_rs.m_query);
				return value;
			}
#endif

			private:
				ResultSet<T>& m_rs;
				int m_position;
	};

	template< class T>
	const T& operator <<( T& target, const ResultSetIterator<T>& source)
	{
		source.to( target);
		return  target;
	}

	/// @brief It is an helper class to iterate over database result sets.
	template< class T>
	class ResultSet
	{
		public:
			friend class ResultSetIterator<T>;
			using iterator = ResultSetIterator<T>;
		
			/// @brief Create an result set from a SQL query.
			/// @param sql SQL query.
			/// @param parent It will be used as a parent for each created object.
			explicit ResultSet( QSqlQuery sql, QObject* parent = nullptr) 
				: m_query( sql), m_parent( parent)
			{ m_query.next(); }

			iterator begin() const
			{ return iterator( const_cast<ResultSet&>(*this), m_query.at()); }

			iterator end() const
			{ return iterator( const_cast<ResultSet&>(*this), 
					static_cast<int>( QSql::AfterLastRow));}

			protected:
				QSqlQuery m_query;	///< Query.
				QObject* m_parent;	///< Parent used for created objects.
	};
}}
