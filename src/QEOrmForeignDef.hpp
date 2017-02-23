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
class QEOrmForeignDef
{
	public:
		QEOrmForeignDef(
			QEOrmModel& targetModel,
			const QByteArray& propertyName,
			const QEOrmModel& reference, 
			const std::vector<QEOrmColumnDef>& primaryKeys);

		const QByteArray& propertyName() const noexcept;
		const std::vector<QEOrmColumnDef>& foreignKeys() const noexcept;
		const std::vector<QEOrmColumnDef>& referenceKeys() const noexcept;

		QEOrmModel reference() const;

	private:
		const QByteArray m_propertyName;
		const QEOrmModel& m_reference;
		std::vector<QEOrmColumnDef> m_refPrimaryKey;
		std::vector<QEOrmColumnDef> m_foreignKey;
};
QE_END_NAMESPACE
