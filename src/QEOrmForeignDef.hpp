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
#include <QEOrmColumnDef.hpp>
#include <QECommon/QEGlobal.hpp>
#include <vector>

QE_BEGIN_NAMESPACE
class QEOrmModel;

/// @brief Foreign key definition.
class QEOrmForeignDef
{
	public:
		using QEOrmColumnDefList = std::vector<QEOrmColumnDefShd>;

		/// @brief Constructor.
		/// @param propertyName Property name.
		/// @param reference Reference entity.
		QEOrmForeignDef(
			const QByteArray& propertyName,
			const std::shared_ptr<QEOrmModel>& reference );

		/// @return It returns the property name.
		const QByteArray& propertyName() const noexcept;

		/// @return It return the foreign key definition.
		const QEOrmColumnDefList& foreignKeys() const noexcept;

		/// @return It return the reference model.
		std::shared_ptr<QEOrmModel> reference() const noexcept;

	private:
		const QByteArray m_propertyName;
		std::shared_ptr<QEOrmModel> m_reference;
		QEOrmColumnDefList m_foreignKey;
};
using QEOrmForeignDefShd = std::shared_ptr<QEOrmForeignDef>;
QE_END_NAMESPACE
