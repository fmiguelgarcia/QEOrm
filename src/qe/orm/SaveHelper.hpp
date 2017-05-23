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
#include <qe/entity/Types.hpp>

namespace qe { namespace orm { 
	class SaveHelperPrivate;
	class SerializedItem;

	class SaveHelper
	{
		public:
			virtual ~SaveHelper();

			void save( entity::ObjectContext& context, 
				const entity::ModelShd& model, QObject *const source, 
				SerializedItem* const target) const;

			QStringList createTables( const entity::ModelShd model, 
				SerializedItem* const target) const; 

		protected:
			void saveOneToMany( entity::ObjectContext& context, 
				const entity::Model &model, QObject *source, 
				SerializedItem* const target) const;

			SaveHelperPrivate *d_ptr;

		private:
			Q_DECLARE_PRIVATE( SaveHelper);
	};
}}
	
