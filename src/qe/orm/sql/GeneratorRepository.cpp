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
#include "GeneratorRepository.hpp"
#include "generator/ANSIGenerator.hpp"
#include "generator/SQLiteGenerator.hpp"
#include "generator/CachedGenerator.hpp"

#include <qe/common/Exception.hpp>
#include <QStringBuilder>
#include <QSqlDriver>

using namespace qe::orm::sql;
using namespace qe::common;
using namespace std;

GeneratorRepository& GeneratorRepository::instance()
{
	static unique_ptr<GeneratorRepository> instance;
	static once_flag onceFlag;
	
	call_once( onceFlag,
			   []{ instance.reset( new GeneratorRepository);});
	return *instance.get();
}
			
GeneratorRepository::GeneratorRepository()
{}

GeneratorRepository::~GeneratorRepository()
{}

AbstractGenerator* GeneratorRepository::createAndStoreGenerator(const int dbmsType)
{
	using GeneratorBuilder = std::function<AbstractGenerator*()>;
	const static map<int, GeneratorBuilder> sqlGenMaker = {
		{ QSqlDriver::UnknownDbms, [](){ return new ANSIGenerator(); } }, 
		{ QSqlDriver::SQLite, [](){ return new SQLiteGenerator();} } 
	};
	
	auto itr = sqlGenMaker.find( dbmsType);
	if( itr == end( sqlGenMaker))
		itr = sqlGenMaker.find( QSqlDriver::UnknownDbms);
	
	AbstractGenerator* generator = itr->second();
	shared_ptr<AbstractGenerator> cachedGenerator = make_shared<CachedGenerator>( generator);

	m_generatorByDriver.insert(
		make_pair( dbmsType, cachedGenerator));
	
	return cachedGenerator.get();
}

AbstractGenerator* GeneratorRepository::generator( const int dbmsType)
{
	AbstractGenerator * generator;
	
	auto itr = m_generatorByDriver.find( dbmsType);
	if( itr != end( m_generatorByDriver))
		generator = itr->second.get();
	else
	{
		lock_guard<mutex> _( m_generatorByDriverMtx);
		itr = m_generatorByDriver.find( dbmsType);
		if( itr != end( m_generatorByDriver))
			generator = itr->second.get();
		else
			generator = createAndStoreGenerator( dbmsType);
	}
	
	if( !generator)
		Exception::makeAndThrow(
			QString( "QE Orm cannot create a SQL generator for DBMS %1")
				.arg( dbmsType));
	
	return generator;
}
