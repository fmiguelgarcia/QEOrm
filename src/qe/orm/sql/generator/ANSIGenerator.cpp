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
#include "ANSIGenerator.hpp"
#include <qe/entity/Model.hpp>
#include <qe/entity/EntityDef.hpp>
#include <qe/entity/RelationDef.hpp>

#include <QDateTime>
#include <QStringBuilder>
#include <QTextStream>
#include <algorithm>

using namespace qe::orm::sql;
using namespace qe::orm;
using namespace qe::entity;
using namespace std;

namespace {
	/// @brief It returns a list of strings contains the DB column names.
	template< class C>
	QStringList columnDbNames( const C& container)
	{
		QStringList colDbNames;
		for( const auto& colDef: container)
			colDbNames << colDef->entityName();
		return colDbNames;	
	}

	uint estimateTypeSize( const EntityDef& eDef){
		const uint nElements = max( eDef.maxLength(), 1u);
		uint typeSize;
		switch( eDef.propertyType())
		{
			case QMetaType::Short:
			case QMetaType::Int:
			case QMetaType::Bool:
			case QMetaType::QChar:
			case QMetaType::Char:
			case QMetaType::SChar:
			case QMetaType::UChar:
			case QMetaType::UShort:
			case QMetaType::UInt:
			case QMetaType::Float:
				typeSize = nElements * 4;
				break;
			case QMetaType::QUuid:
			case QMetaType::Long:
			case QMetaType::LongLong:
			case QMetaType::ULong:
			case QMetaType::ULongLong:
			case QMetaType::Double:
				typeSize = nElements * 8;
				break;
			case QMetaType::QDate:
			case QMetaType::QTime:
			case QMetaType::QDateTime:
				typeSize = nElements * 16;
				break;
			case QMetaType::QString:
			case QMetaType::QByteArray:
			case QMetaType::QJsonValue:
			case QMetaType::QJsonObject:
			case QMetaType::QJsonArray:
			case QMetaType::QJsonDocument:
			default:
				if( eDef.maxLength() == 0)
					typeSize = std::numeric_limits<uint>::max();
				else
					typeSize = nElements * 1000;
		}
		return typeSize;
	}
	
	struct TypeSizeLessComparator {
		inline bool operator()( const EntityDefShd& left, const EntityDefShd& right) const
		{ return estimateTypeSize(*left) < estimateTypeSize(*right);}
	};
}

ANSIGenerator::~ANSIGenerator()
{}

QString ANSIGenerator::existsStatement( const Model& model) const
{
	const QString stmt = 
		QStringLiteral( "SELECT * FROM '") 
		% model.name()
	  	% QStringLiteral("' WHERE ")
		% primaryKeyWhereClausure( model) 
		% QStringLiteral( " LIMIT 1");
	
	return stmt;
}

QString ANSIGenerator::updateStatement( const Model& model) const
{
	QStringList setClausure;
	EntityDefList noPrimaryKeyDefs;

	// Get NO primary key entities and no mapping	
	const EntityDefList &pkDef = model.primaryKeyDef();

	// Generate clause
	for( const auto& eDef: model.entityDefs())
	{
		if( find( begin( pkDef), end( pkDef), eDef) != end( pkDef))
			continue;
		
		if( eDef->mappingType() == EntityDef::MappingType::NoMappingType)
		{
			setClausure << (eDef->entityName()
				% QStringLiteral( " = :") 
				% eDef->entityName());
		}
	}

	// Compose statement.	
	const QString stmt = 
		QStringLiteral( "UPDATE '") % model.name() % QStringLiteral( "' SET ") 
		% setClausure.join( QStringLiteral(", "))
		% QStringLiteral( " WHERE ") 
		% primaryKeyWhereClausure( model);

	return stmt;
}

QString ANSIGenerator::insertStatement( const Model& model) const
{
	QStringList values;
	QStringList columnNames;

	for( const auto& eDef: model.entityDefs())
	{
		if( eDef->mappingType() == EntityDef::MappingType::NoMappingType
			|| eDef->mappingType() == EntityDef::MappingType::ManyToOne)
		{
			const QString & dbColName = eDef->entityName();
			columnNames << dbColName;
			values << QChar(':') % dbColName;
		}
	}
	
	const QString stmt = 
		QStringLiteral( "INSERT INTO '") % model.name() 
		% QStringLiteral("' (") % columnNames.join( QStringLiteral(", ")) 
		% QStringLiteral(") VALUES (") % values.join( QStringLiteral(", ")) 
		% QStringLiteral(")");

	return stmt;
}

QString ANSIGenerator::createTableIfNotExistsStatement( const Model& model) const
{
	QStringList sqlColumnDef;

	// Generate SQL for each column.
	EntityDefList entityDefsOrdered = model.entityDefs();
	sort( begin( entityDefsOrdered), end( entityDefsOrdered), TypeSizeLessComparator());
	
	for( const auto& eDef: entityDefsOrdered )
	{
		if( eDef->mappingType() == EntityDef::MappingType::NoMappingType
			|| eDef->mappingType() == EntityDef::MappingType::ManyToOne)
		{
			sqlColumnDef << makeColumnDefinition( *eDef);
		}
	}

	const QString primaryKeyDef = makePrimaryKeyDefinition( model);
	const QString foreignKeyDef = makeForeignKeyDefinition( model);

	// Generate SQL.
	const QString stmt = 
		QStringLiteral( "CREATE TABLE IF NOT EXISTS '") 
		% model.name() % QStringLiteral("' (") % sqlColumnDef.join( QStringLiteral(", "))
		% primaryKeyDef 
		% foreignKeyDef 
		% QStringLiteral( ")");

	return stmt;
}

QString ANSIGenerator::selectionUsingPrimaryKey( const Model& model) const
{
	const QString stmt =
		projection( model)
		% QStringLiteral( " WHERE ") 
		% primaryKeyWhereClausure( model);

	return stmt;
}

QString ANSIGenerator::selectionUsingProperties( const Model& model,
	const EntityDefList& entities) const
{
	const QString stmt =
		projection( model)
		% QStringLiteral( " WHERE ") 
		% whereClausure( entities);

	return stmt;
}

QString ANSIGenerator::projection( const Model &model) const
{
	// Get no mapping columns
	QStringList columns;

	EntityDefList entityDefsOrdered = model.entityDefs();
	sort( begin( entityDefsOrdered), end( entityDefsOrdered), TypeSizeLessComparator());
	
	for( const auto& eDef : entityDefsOrdered)
		if( eDef->mappingType() == EntityDef::MappingType::NoMappingType)
			columns << eDef->entityName();

	const QString stmt = 
		QStringLiteral( "SELECT ") 
	  	% columns.join( QStringLiteral(", "))
		% QStringLiteral( " FROM '") % model.name() % QStringLiteral("' ");

	return stmt;
}

QString ANSIGenerator::selectionUsingForeignKey( const Model& model, 
		const RelationDef& fkDef) const
{
	const QString stmt = 
		projection( model) 
		% QStringLiteral( " WHERE ") 
		% foreignKeyWhereClausure( fkDef);

	return stmt;
}

QString ANSIGenerator::makeColumnDefinition( const EntityDef& column) const
{
	QString sqlColumnDef;
	QTextStream os( &sqlColumnDef);
	const QLatin1Char quotationMark ('\'');
	const QLatin1Char space(' ');
		
	// Name
	os << quotationMark << column.entityName() << quotationMark << space;
		
	// type 
	os << databaseType( column.propertyType(), column.maxLength()) << space;
	
	// Null
	os << (( column.isNullable() )
		? QStringLiteral( "NULL ")
		: QStringLiteral( "NOT NULL "));
		
	// Default value
	if( ! column.defaultValue().isNull() )
		os << QStringLiteral( "DEFAULT ") << column.defaultValue().toString() << space;
		
	// Auto-increment
	if( column.isAutoIncrement())
		os << autoIncrementKeyWord() << space; 
	
	return sqlColumnDef;
}

QString ANSIGenerator::databaseType(const int propertyType, 
		const uint size) const
{
	QString dbType;
	switch( propertyType)
	{
		case QMetaType::Bool:
			dbType = QStringLiteral( "BOOL");
			break;
		case QMetaType::QChar:
		case QMetaType::Char:
		case QMetaType::SChar:
		case QMetaType::UChar:
			dbType = QStringLiteral( "CHAR");
			break;	
		case QMetaType::UShort:
		case QMetaType::UInt:
			dbType = QStringLiteral( "INT UNSIGNED");
			break;	
		case QMetaType::Long:
		case QMetaType::LongLong:
			dbType = QStringLiteral( "BIGINT");
			break;	
		case QMetaType::ULong:
		case QMetaType::ULongLong:
			dbType = QStringLiteral( "BIGINT UNSIGNED");
			break;	
		case QMetaType::Float:
			dbType = QStringLiteral( "FLOAT");
			break;
		case QMetaType::Double:
			dbType = QStringLiteral( "DOUBLE");
			break;
		case QMetaType::QString:
			dbType = (( size == 0)
				? QStringLiteral( "TEXT")
				: QString( "VARCHAR(%1)").arg( size));
			break;
		case QMetaType::QByteArray:
			dbType = ( size == 0)
				? QStringLiteral( "BLOB")
				: QString( "VARBINARY(%1)").arg( size);
			break;
		case QMetaType::QTime:
			dbType = QStringLiteral( "TIME");
			break;	
		case QMetaType::QDate:
			dbType = QStringLiteral( "DATE");
			break;	
		case QMetaType::QDateTime:
			dbType = QStringLiteral( "DATETIME");
			break;	
		case QMetaType::QJsonValue:
		case QMetaType::QJsonObject:
		case QMetaType::QJsonArray:
		case QMetaType::QJsonDocument:
			dbType = QStringLiteral( "JSON");
			break;	
		case QMetaType::QUuid:
			dbType = QStringLiteral( "BINARY(16)");
			break;
		case QMetaType::Short:
		case QMetaType::Int:
		default:
			dbType = QStringLiteral( "INT");
	}
	return dbType;
}

QString ANSIGenerator::primaryKeyWhereClausure( const Model &model) const 
{ return whereClausure(  model.primaryKeyDef()); }

QString ANSIGenerator::whereClausure( const EntityDefList &colDefList) const
{
	QStringList pkWhereClause;
	for( const auto& colDef : colDefList) 
		pkWhereClause << ( colDef->entityName() 
				% QStringLiteral(" = :") 
				% colDef->entityName()); 
	
	return pkWhereClause.join( QStringLiteral(" AND "));
}

QString ANSIGenerator::autoIncrementKeyWord() const
{ return QStringLiteral( "AUTO_INCREMENT"); }

QString ANSIGenerator::makePrimaryKeyDefinition(const Model &model)  const
{
	QString pkDef;
	const auto pk = model.primaryKeyDef();
	if( ! pk.empty())
	{
		const QStringList pkColumNames = columnDbNames(pk);
		pkDef = QStringLiteral( ", PRIMARY KEY (")
			% pkColumNames.join( QStringLiteral( ", "))
			% QStringLiteral(")");
	}
	return pkDef;
}

QString ANSIGenerator::makeForeignKeyDefinition( const Model& model) const
{
	QString fkDef;
	QTextStream os( &fkDef);
	const QChar comma = QLatin1Char(',');

	for( const auto& fkDef : model.referencesManyToOneDefs())
	{
		const auto reference = fkDef->reference();
		const QStringList fkColNames = columnDbNames( fkDef->relationKey());
		const QStringList refColNames = columnDbNames( reference->primaryKeyDef());

		os << QStringLiteral( ", FOREIGN KEY (") << fkColNames.join( comma)  
			<< QStringLiteral( ") REFERENCES ") << reference->name() << QLatin1Char('(')
			<< refColNames.join( comma) << QLatin1Char(')')
			<< QStringLiteral( " ON DELETE CASCADE")
			<< QStringLiteral( " ON UPDATE CASCADE");
	}
	return fkDef;
}

QString ANSIGenerator::foreignKeyWhereClausure( const RelationDef& fkDef) const 
{
	const EntityDefList fkList = fkDef.relationKey();
	const EntityDefList pkList = fkDef.reference()->primaryKeyDef();

	QStringList filters;
	for( uint i = 0; i< fkList.size(); ++i)
		filters << (fkList[i]->entityName()
				% QStringLiteral( " = :")
				% pkList[i]->entityName());

	return filters.join( QStringLiteral( " AND "));
}

QString ANSIGenerator::deleteStatement( const Model& model) const
{
	const QString stmt =
		QStringLiteral( "DELETE FROM '") 
		% model.name() 
		% QStringLiteral( "' WHERE ") 
		% primaryKeyWhereClausure( model);

	return stmt;
}
