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
#include "QEOrmModel.hpp"
#include <QEAnnotation/QEAnnotationModel.hpp>
#include <QMetaProperty>
#include <utility>
#include <memory>

using namespace std;
namespace {
	inline QString ANN_CLASS_ID() 
	{ return QLatin1Literal( "class");}
	inline QString ANN_TABLE_KEY() 
	{ return QLatin1Literal( "@QEOrm.table");}
	inline QString ANN_TEMPORARY_TABLE_KEY() 
	{ return QLatin1Literal( "@QEOrm.isTemporaryTable");}
	
	inline QString ANN_EXPORT_PARENT_KEY() 
	{ return  QLatin1Literal( "@QEOrm.isParentExported");}
	inline QString ANN_PRIMARY_KEY() 
	{ return QLatin1Literal( "@QEOrm.primaryKey");}
	inline QString ANN_INDEX() 
	{ return QLatin1Literal( "@QEOrm.index");}
	inline QString ANN_ENABLE() 
	{ return QLatin1Literal( "@QEOrm.isEnable");}
	
	/// @brief It gets the primary keys from annotation.
	/// 
	/// If there is no explicit primary key, it will use the first
	/// 'auto_increment' file as a primary key.
	QEOrmModel::QEOrmColumnDefList parsePrimaryKeys( const QEOrmModel & model)
	{
		QEOrmModel::QEOrmColumnDefList pk;
		QStringList pkPropertyNames = model.annotation( ANN_CLASS_ID(), ANN_PRIMARY_KEY())
			.value( QString()).toString()
			.split( ',', QString::SkipEmptyParts);

		if( pkPropertyNames.isEmpty())
		{
			QEOrmColumnDefShd colDef = model.findColumnDef( QEOrmModel::findByAutoIncrement{});
			if( colDef)
				pkPropertyNames << colDef->propertyName;
		}

		for( QString pkPropName: pkPropertyNames)
		{
			QEOrmColumnDefShd colDef = model.findColumnDef( QEOrmModel::findByPropertyName{pkPropName});
			if( colDef )
			{
				pk.push_back( colDef);
				colDef->isPartOfPrimaryKey = true;
			}
		}

		return pk;
	}
}

// Class QEOrmModel
// ============================================================================

QEOrmModel::QEOrmModel( const QMetaObject* metaObj)
	: QEAnnotationModel( metaObj) 
{
	parseAnnotations( metaObj);
}

const QString& QEOrmModel::table() const noexcept
{ return m_table; }

const QEOrmModel::QEOrmColumnDefList& QEOrmModel::columnDefs() const noexcept
{ return m_columnDefs; }

const QEOrmModel::QEOrmColumnDefList& QEOrmModel::primaryKeyDef() const noexcept
{ return m_primaryKeyDef; }

const QEOrmModel::QEOrmForeignDefList& QEOrmModel::referencesManyToOneDefs() const noexcept
{ return m_referencesManyToOneDefs; }

void QEOrmModel::addReferenceManyToOne( const QByteArray& propertyName, 
		const QEOrmModelShd &reference)
{
	QEOrmForeignDefShd fkDef = make_shared<QEOrmForeignDef>( propertyName, reference);

	// Fix fk column names to avoid duplicates.
	QString fkColName;
	for( auto & fkColDef	: fkDef->foreignKeys())
	{
		fkColName = fkColDef->dbColumnName;
		auto colDef = findColumnDef( findByColumnName{ fkColName });
		if( colDef )
		{
			uint colissionIdx = 0;
			fkColName = QString("%1_%2").arg( reference->table())
				.arg( fkColDef->dbColumnName);
			colDef =  findColumnDef( findByColumnName{ fkColName });
			while( colDef )
			{
				fkColName = QString("%1_%2_%3").arg( reference->table())
					.arg( fkColDef->dbColumnName).arg( ++colissionIdx);
				colDef = findColumnDef( findByColumnName{ fkColName });
			}
		}
		fkColDef->dbColumnName = fkColName;
		m_columnDefs.push_back( fkColDef);
	}

	// Copy ref
	m_referencesManyToOneDefs.push_back( fkDef);
}


void QEOrmModel::parseAnnotations( const QMetaObject* metaObj)
{
	// Table
	m_table = annotation( ANN_CLASS_ID(), ANN_TABLE_KEY())
		.value( QString( metaObj->className())).toString();
	// Columns			
	const bool exportParents = annotation( ANN_CLASS_ID(), ANN_EXPORT_PARENT_KEY())
		.value( false).toBool();
	const int begin = (exportParents) ? 0 : metaObj->propertyOffset();
	for( int i = begin; i < metaObj->propertyCount(); ++i)
	{
		QMetaProperty property = metaObj->property(i);
		const QByteArray propertyName = property.name();
		const bool isEnable = annotation( propertyName, ANN_ENABLE())
			.value( true).toBool();
		if( isEnable )
		{
			QEOrmColumnDefShd colDef = make_shared<QEOrmColumnDef>( propertyName, property.type(), *this);
			m_columnDefs.push_back( colDef);
		}
	}
	
	m_primaryKeyDef = parsePrimaryKeys( *this);
}

QEOrmColumnDefShd QEOrmModel::findColumnDef( QEOrmModel::FindColDefPredicate&& predicate) const noexcept
{
	QEOrmColumnDefShd column;
	const auto itr = find_if( begin( m_columnDefs), end( m_columnDefs), predicate);
	if( itr != end( m_columnDefs))
		column = *itr;

	return column;
}

QEOrmColumnDefShd QEOrmModel::findColumnDef(const QEOrmModel::findByPropertyName& property) const noexcept
{
	return findColumnDef( 
			[&property]( const QEOrmColumnDefShd& colDef) -> bool
				{ return colDef->propertyName == property.name;});
}

QEOrmColumnDefShd QEOrmModel::findColumnDef(const QEOrmModel::findByColumnName& column) const noexcept
{
	return findColumnDef( 
			[&column]( const QEOrmColumnDefShd& colDef)
				{ return colDef->dbColumnName == column.name;});
}

QEOrmColumnDefShd QEOrmModel::findColumnDef( const QEOrmModel::findByAutoIncrement& ) const noexcept
{
	return findColumnDef( 
			[]( const QEOrmColumnDefShd& colDef)
				{ return colDef->isDbAutoIncrement;});
}

const QEOrmForeignDefShd QEOrmModel::findForeignTo( const QEOrmModelShd& model) const noexcept
{
	QEOrmForeignDefShd fk;

	const auto itr = find_if( begin( m_referencesManyToOneDefs), end( m_referencesManyToOneDefs),
			[model]( const QEOrmForeignDefShd& fkDef){
				return fkDef->reference() = model;});
	if( itr != end( m_referencesManyToOneDefs))
		fk = *itr;

	return fk;
}

