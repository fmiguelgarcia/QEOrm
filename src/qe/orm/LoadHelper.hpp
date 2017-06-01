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
#include <qe/entity/Types.hpp>
#include <QVariantList>

class QSqlRecord;
namespace qe { namespace orm { 
	class LoadHelperPrivate;	
	class S11nContext;

	class LoadHelper 
	{
		public:
			virtual ~LoadHelper();

			void load( 
				const entity::ModelShd& model, 
				const S11nContext* const context, 
				QObject *const target) const;

			void loadObjectFromRecord( 
				const entity::Model& model, 
				const QSqlRecord& record, 
				QObject *const target) const;

			void loadOneToMany(
				const entity::ModelShd& model,
				const S11nContext *const source,
				QObject* const target) const;

		protected:
			LoadHelperPrivate * d_ptr;

		private:
			QVariantList loadObjectsUsingForeignKey( 
				const entity::Model& refModel,
				const entity::RelationDef& fkDef, 
				const S11nContext* const source,
				const QMetaObject* refMetaObjEntity, 
				QObject* const target) const;

			Q_DECLARE_PRIVATE( LoadHelper);
	};
}}
