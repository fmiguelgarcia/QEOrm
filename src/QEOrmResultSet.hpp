#pragma once
#include <helpers/QEOrmLoadHelper.hpp>
#include <QObject>
#include <QMetaObject>
#include <QSqlQuery>
#include <iterator>

template< class T>
class QEOrmResultSet
{
	public:
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

				T* operator *() const
				{ 
					T* value = reinterpret_cast<T*>( T::staticMetaObject.newInstance( Q_ARG(QObject*, m_rs.m_parent)));
					QEOrmLoadHelper::load( value, m_rs.m_query);
					return value; 
				}

			private:
				QEOrmResultSet& m_rs;
				int position;
		};

		explicit QEOrmResultSet( QSqlQuery sql, QObject* parent = nullptr) 
			: m_query( sql), m_parent( parent)
		{ m_query.first(); }

		iterator begin() const
		{ return iterator( const_cast<QEOrmResultSet&>(*this), 0); }

		iterator end() const
		{ return iterator( const_cast<QEOrmResultSet&>(*this), -1); }

	protected:
		QSqlQuery m_query;
		QObject* m_parent;
};

