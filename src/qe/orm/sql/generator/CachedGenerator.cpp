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
 */

#include "CachedGenerator.hpp"

using namespace qe::orm::sql;
using namespace qe::entity;
using namespace std;

namespace {
	struct Model2StmtValueLessComparator {
		bool operator()( const CachedGenerator::Model2StmtValue& left,
			const CachedGenerator::Model2StmtValue& right) const 
		{ return left.model < right.model; }
	};
}

CachedGenerator::CachedGenerator( AbstractGenerator* generator)
	:m_generator( generator)
{}

CachedGenerator::~CachedGenerator()
{}

CachedGenerator::Model2StmtValue CachedGenerator::findByModel( 
	const CachedQuery cq, const Model& model, 
	const function<QString ()> strGenerator) const
{
	Model2StmtValueLessComparator less;
	Model2StmtValue targetValue { &model };
	Model2StmtValue value;

	Model2Stmt& refStmtByModel = m_stmtByModel[ static_cast<int>(cq)];
	auto range = equal_range( begin(refStmtByModel), end(refStmtByModel),
			targetValue, less); 
	if( range.first == range.second)
	{
		lock_guard<mutex> _(m_stmtByModelMtx);
		range = equal_range( begin(refStmtByModel), end(refStmtByModel), 
			targetValue, less);
		if( range.first == range.second)
		{
			value = Model2StmtValue{ &model, strGenerator()};
			refStmtByModel.push_back( value); 
			sort( begin(refStmtByModel), end(refStmtByModel), less);
			return value;
		}
	}
		
	value = *range.first;
	return value;
}

// Cached statements
// =============================================================================

QString CachedGenerator::existsStatement( const Model& model) const
{
	return findByModel( cqExistStatement, model,
			[this, &model]() -> QString
			{ return m_generator->existsStatement( model);})
		.cachedValue;
}
			
QString CachedGenerator::updateStatement( const Model& model) const
{
	return findByModel( cqUpdateStatement, model,
			[this, &model]() -> QString
			{ return m_generator->updateStatement( model);})
		.cachedValue;
}
			
QString CachedGenerator::insertStatement( const Model& model) const
{
	return findByModel( cqInsertStatement, model,
			[this, &model]() -> QString
			{ return m_generator->insertStatement( model);})
		.cachedValue;
}

QString CachedGenerator::selectionUsingPrimaryKey( 
	const Model& model) const
{
	return findByModel( cqSelectionUsingPrimaryKey, model,
			[this, &model]() -> QString
			{ return m_generator->selectionUsingPrimaryKey( model);})
		.cachedValue;
}

QString CachedGenerator::projection( const Model &model) const
{
	return findByModel( cqProjection, model,
			[this, &model]() -> QString
			{ return m_generator->projection( model);})
		.cachedValue;
}

QString CachedGenerator::primaryKeyWhereClausure( 
	const Model &model) const 
{
	return findByModel( cqPrimaryKeyWhereClausure, model,
			[this, &model]() -> QString
			{ return m_generator->primaryKeyWhereClausure( model);})
		.cachedValue;
}

// No Cached statements
// ============================================================================

QString CachedGenerator::createTableIfNotExistsStatement( 
	const Model& model) const
{ return m_generator->createTableIfNotExistsStatement( model); }

QString CachedGenerator::selectionUsingForeignKey( const Model& model, 
	const RelationDef& fkDef) const
{ return m_generator->selectionUsingForeignKey( model, fkDef);}

QString CachedGenerator::selectionUsingProperties( const Model& model,
	const EntityDefList& entities) const 
{ return m_generator->selectionUsingProperties( model, entities);}

QString CachedGenerator::foreignKeyWhereClausure( const RelationDef& fkDef) const
{ return m_generator->foreignKeyWhereClausure( fkDef);}

QString CachedGenerator::whereClausure( const EntityDefList &colDefList) const
{ return m_generator->whereClausure( colDefList);}

QString CachedGenerator::autoIncrementKeyWord() const
{ return m_generator->autoIncrementKeyWord(); }

QString CachedGenerator::makeColumnDefinition( const EntityDef& column) const
{ return m_generator->makeColumnDefinition( column);}

QString CachedGenerator::makePrimaryKeyDefinition( const Model &model) const
{ return m_generator->makePrimaryKeyDefinition( model);}

QString CachedGenerator::makeForeignKeyDefinition( const Model& model) const
{ return m_generator->makeForeignKeyDefinition( model);}

QString CachedGenerator::databaseType( const int propertyType, const uint size) const
{ return m_generator->databaseType( propertyType, size);}
