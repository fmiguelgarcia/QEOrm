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

/**
 * @section ORM_DESCRIPTION ORM General Description 
 * QE ORM is a Object-relational mapping (ORM, O/RM, or O/R mapping tool) that allows you to map
 * your QObject subclasses to a SQL database.
 * It uses Qt Property system to identified what properties of an object should be mapped to specific 
 * database table columns.
 * It also uses QEAnnotation library to add customizations to those clases or properties and fit them 
 * to database constraints like primary keys, unique, or not null.
 * Let's see an simple example, a 'user' table that we will use in authentication process:
 * @code{.cpp}
 * // DbUser.hpp
 * #include <QObject>
 * class DBUser : public QObject
 * {
 * 	Q_OBJECT
 * 	Q_PROPERTY( int id MEMBER id)
 * 	Q_PROPERTY( QString name MEMBER name)
 * 
 * 	Q_CLASSINFO( "class", "@QEOrm.table=myUser ")
 * 	Q_CLASSINFO( "id", "@QEOrm.isAutoIncrement=true")
 * 	Q_CLASSINFO( "name", "@QEOrm.isNull=false @QEOrm.maxLength=255)
 *
 * 	public:
 * 		DBUser( QObject *parent = nullptr);
 * 	
 * 		int id;
 * 		QString name;
 * };
 * @endcode
 * In the above code, we have used the @c Q_CLASSINFO to add the following annotations:
 *  # 
 * 
 * @subsection ORM_CLASS_ANNOTATION ORM Class Annotation
 * Those are annotation which are refered to the class. All of them MUST USE @c "class" as key for @c Q_CLASSINFO :
 *  - @c @QEOrm.table: It is a string contains the database table name used to store object of this class. 
 * 	 By default, QE ORM uses the class name as a database table name.
 *  - @c @QEOrm.isTemporaryTable: It creates the database table as a temporary one. By default, it is @c false.
 *  - @c @QEOrm.isParentExported: If it is @c true, it will also export properties from parents classes. By default, it is @c false.
 *  - @c @QEOrm.primaryKey: It is a comma-separated list of properties which will be used as a primary key. By
 * 	default, no primary key is defined unless an @c auto_increment field is found.
 * 
 * @subsection ORM_COLUMN_ANNOTATION ORM Column Annotation
 * This section describes the annotation we could use to define in properties of an object.
 * - @c @QEOrm.isAutoIncrement: If it is @c true, the property is marked as an @c auto_increment. In this case, 
 * 	QE ORM considers 0 value as a @c null, so database will generate a proper value. After the insertion, 
 * 	this object property will be updated with the new value of the insertion. By default, it is @c false.
 * - @c @QEOrm.dbColumn: It is the name of the column that will be used to store this property. By default,
 * 	QE ORM will use the name of the property as a column name.
 * - @c @QEOrm.maxLength: In @c QString properties, you can specify the maximun length for database column. If you do not 
 * 	assign any one, the underline SQL database will be used to defined the real type for the database column. For instance,
 * 	if SQLite is used as database, and you do not define any value for this, a @c TEXT column type will be used to store this property.
 * - @c @QEOrm.isNull: If this is @c false, the column table will be created as @c NOT @c NULL. By default, it is @c true.
 */
#pragma once
#include <qe/entity/serialization/AbstractSerializer.hpp>
#include <qe/entity/Types.hpp>
#include <QVariantList>
#include <set>
#include <mutex>

namespace qe { namespace orm {
	
	/// @todo Add support for QSqlQuery::isForwardOnly
	/// @todo Check table version. 
	/// @brief It is the helper class to save and load objects from database.
	/// It also create the tables if they does not exist on database.
	/// It uses the default database connection.
	class QEOrm : public qe::entity::AbstractSerializer
	{
		public:
			/// @brief It returns the singleton instance.
			static QEOrm& instance();

			void save( QObject* const source) const; 

			void load( QVariantList&& primaryKey, 
					QObject *const target) const;

#if 0 
			/// @brief It finds all database objects which properties are equal to
			/// values specified on @p properties map.
			/// @param properties Properties map.
			/// @param parent Parent will be used for creation of each new object. If
			/// nullptr is used, you should be responsible to delete them.
			template< class T>
				ResultSet<T> findEqual( 
					const std::map<QString, QVariant>& properties,
					QObject* parent = nullptr) const
				{
					const QMetaObject* mo = & T::staticMetaObject;
					QEOrmFindHelper findHelper( mo, m_sqlGenerator.get());		
					return QEOrmResultSet<T>( findHelper.findEqualProperty(properties), parent);
				}
#endif

		protected:
			void save( qe::entity::ObjectContext& context, 
					const qe::entity::ModelShd& model, QObject *const source, 
					qe::entity::AbstractSerializedItem* const target) const override;

			void load( qe::entity::ObjectContext& context, 
					const qe::entity::ModelShd& model, 
					const qe::entity::AbstractSerializedItem *const source,
					QObject *const target) const override;

		private:
			QEOrm();
			QEOrm( const QEOrm& );
			QEOrm& operator=( const QEOrm& );

		private:
			mutable std::mutex m_checkedTablesMtx;
			mutable std::set<QString> m_checkedTables;
	};
}}
