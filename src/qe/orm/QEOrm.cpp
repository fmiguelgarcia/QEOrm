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
#include "QEOrm.hpp"
#include "SaveHelper.hpp"
#include "LoadHelper.hpp"
#include "DeleteHelper.hpp"
#include "S11nContext.hpp"
#include <qe/orm/sql/Executor.hpp>
#include <qe/common/Exception.hpp>
#include <qe/entity/Model.hpp>
#include <qe/entity/ModelRepository.hpp>
#include <QStringList>
#include <QStringBuilder>

using namespace qe::orm;
using namespace qe::entity;
using namespace qe::common;
using namespace std;

namespace {
	/// @brief Utility to use double check locking and create/insert objects
	/// into a container.
	template< typename C, typename K, typename M>
	bool existsOrCreateUsingDoubleCheckeLocking( C &&container, K&& key, M&& mutex)
	{
		auto itr = container.find( key);
		if( itr == std::end( container))
		{
			std::lock_guard<M> _( std::forward<M>(mutex));
			itr = container.find( key);
			if( itr == std::end( container))
			{
				container.insert( key);
				return false;
			}
		}
		return true;
	}

	template< class T, class M>
	void checkAndCreateDatabaseTables(
			const qe::orm::SaveHelper& saveHelper,
			const ModelShd& model,
			S11nContext* const context,
			T& checkedTables,
			M& checkedTablesMtx) 
	{
		const bool isAlreadyChecked = existsOrCreateUsingDoubleCheckeLocking( 
				checkedTables, model->name(), checkedTablesMtx);
		if( !isAlreadyChecked)
		{
			const QStringList tables = saveHelper.createTables( model, context);
			for( const QString& table: tables)
				existsOrCreateUsingDoubleCheckeLocking(
					  	checkedTables, table, checkedTablesMtx);
		}
	}

	ModelShd getModelOrThrow( const QMetaObject* metaObject)
	{
		ModelShd model = ModelRepository::instance().model( metaObject);
		if( !model)
			Exception::makeAndThrow(
					QStringLiteral( "QE Orm cannot find model for class ")
					% metaObject->className());

		return model;
	}
	
	void checkAndCreateModel( 
		const ModelShd& model, 
		const S11nContext* const context,
		std::set<QString>& checkedTables,
		std::mutex & checkedTablesMtx)
	{
		SaveHelper saver;
		checkAndCreateDatabaseTables( 
				saver, 
				model, 
				const_cast<S11nContext*>(context), 
				checkedTables, 
				checkedTablesMtx);
	}

	class SecuredS11Context
	{
		public:
			explicit SecuredS11Context( const AbstractS11nContext* context)
				: m_context( dynamic_cast<const S11nContext*>( context))
			{
				if( !m_context)
				{
					m_contextScopeGuard.reset( new S11nContext);
					m_context = m_contextScopeGuard.get();
				}
			}

			inline S11nContext* get() noexcept
			{ return const_cast<S11nContext*>( m_context); }
			
			inline const S11nContext* get() const noexcept
			{ return m_context; }

		private:
			const S11nContext * m_context;
			unique_ptr<const S11nContext> m_contextScopeGuard;
	};
}

/** @class QEOrm::FindValidatedInputs
 *
 */

QEOrm::FindValidatedInputs::FindValidatedInputs( 
	const QMetaObject* mo, 
	S11nContext* const ctx,
	std::set<QString>& checkedTables,
	std::mutex & checkedTablesMtx)
		: model( getModelOrThrow( mo)), 
		context( ctx)
{
	if( !context)
	{
		contextScopeGuard.reset( new S11nContext);
		context = contextScopeGuard.get();
	}

	checkAndCreateModel( model, context, checkedTables, checkedTablesMtx);
}


/** \class QEOrm 
 *
 */
QEOrm &QEOrm::instance()
{
	static unique_ptr<QEOrm> instance;
	static once_flag onceFlag;

	call_once( onceFlag,
			   []{ instance.reset( new QEOrm);});
	return *instance.get();
}

QEOrm::QEOrm()
{}

// Save operations
// =================================================================

void QEOrm::save( 
	const ModelShd& model, 
	QObject *const source, 
	AbstractS11nContext* const context) const
{
	SecuredS11Context sc( context);
	SaveHelper saver;

	checkAndCreateDatabaseTables( 
		saver, model, sc.get(), m_checkedTables, m_checkedTablesMtx);
	saver.save( model, source, sc.get());
}

// Load operations
// =================================================================

void QEOrm::load( 
	const ModelShd& model, 
	QObject *const target,
	const AbstractS11nContext*const context) const
{
	const SecuredS11Context sc( context);
	LoadHelper loader;
	loader.load( model, sc.get(), target);
}

// Delete operations
// =================================================================

void QEOrm::erase( 
	QObject* const source, 
	S11nContext* const context) const 
{
	SecuredS11Context sc( context);
	DeleteHelper dh;
	ModelShd model = ModelRepository::instance().model( source->metaObject());
	
	dh.erase( model, source, sc.get());
}

// Other stuff
// ==================================================================

QSqlQuery QEOrm::nativeQuery( 
	const S11nContext *const context, 
	const QString& stmt ) const
{
	const QString errorMsg = QStringLiteral("QE Orm cannot execute query.");

	/// @todo Replace property names by column names.
	QSqlQuery ds = context->execute( stmt, QVariantList(), errorMsg);

	return ds;
}
