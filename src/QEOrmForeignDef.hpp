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
			const QEOrmModel& reference, 
			const std::vector<QEOrmColumnDef>& primaryKeys);

		const std::vector<QEOrmColumnDef>& foreignKeys() const noexcept;
		const std::vector<QEOrmColumnDef>& referenceKeys() const noexcept;

#if 0
	  	uint size() const noexcept;
		QEOrmColumnDef foreignKey( const uint idx = 0) const;
		QEOrmColumnDef referenceKey( const uint idx = 0) const;
#endif

		QEOrmModel reference() const;

	private:
		const QEOrmModel& m_reference;
		std::vector<QEOrmColumnDef> m_refPrimaryKey;
		std::vector<QEOrmColumnDef> m_foreignKey;
};
QE_END_NAMESPACE
