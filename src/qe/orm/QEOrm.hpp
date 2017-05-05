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
#include <qe/orm/Global.hpp>
#include <qe/entity/AbstractSerializer.hpp>
#include <qe/entity/Types.hpp>
#include <qe/orm/ResultSet.hpp>
#include <qe/orm/FindHelper.hpp>
#include <QVariantList>
#include <set>
#include <mutex>

namespace qe { namespace orm {
	
	/// @todo Add support for QSqlQuery::isForwardOnly
	/// @todo Check table version. 
	/// @brief It is the helper class to save and load objects from database.
	/// It also create the tables if they does not exist on database.
	/// It uses the default database connection.
	class QEORM_EXPORT QEOrm : public qe::entity::AbstractSerializer
	{
		public:
			/// @brief It returns the singleton instance.
			static QEOrm& instance();

			void save( QObject* const source) const;

			virtual void save( QObject* const source,
				SerializedItem* const target) const;

			void load( QVariantList&& primaryKey, 
				QObject *const target) const;

			void load( const SerializedItem* const si,
				QVariantList&& primaryKey,
				QObject* const target) const;

			/// @brief It deletes @p source object from database specified 
			///	in @p target.
			void erase( QObject* const source,
				SerializedItem* const target) const;
			
			void erase( QObject* const source) const;

			/// @brief It finds all database objects which properties are equal to
			/// values specified on @p properties map.
			/// @param properties Properties map.
			/// @param parent Parent will be used for creation of each new object. If
			/// nullptr is used, you should be responsible to delete them.
			template< class T>
				ResultSet<T> findEqual(
					const SerializedItem * const source,
					const std::map<QString, QVariant>& properties,
					QObject* parent = nullptr) const
				{
					entity::ModelShd model = getModelOrThrow( & T::staticMetaObject);
					checkAndCreateModel( model, source);
					
					FindHelper findHelper;
					return ResultSet<T>( 
						findHelper.findEqualProperty( *model, source, properties), 
						parent);
				}

		private:	
			void checkAndCreateModel( const entity::ModelShd& model,
				const SerializedItem* const target) const;

		protected:
			void save( qe::entity::ObjectContext& context, 
					const qe::entity::ModelShd& model, QObject *const source, 
					qe::entity::AbstractSerializedItem* const target) const override;
			void save( qe::entity::ObjectContext& context, 
					const qe::entity::ModelShd& model, QObject *const source, 
					SerializedItem* const target) const;


			void load( qe::entity::ObjectContext& context, 
					const qe::entity::ModelShd& model, 
					const qe::entity::AbstractSerializedItem *const source,
					QObject *const target) const override;

		private:
			QEOrm();
			QEOrm( const QEOrm& );
			QEOrm& operator=( const QEOrm& );

			entity::ModelShd getModelOrThrow( const QMetaObject* metaObject) const;

		private:
			mutable std::mutex m_checkedTablesMtx;
			mutable std::set<QString> m_checkedTables;
	};
}}
