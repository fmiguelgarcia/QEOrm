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

#include "QEOrmModelPrivate.hpp"
#include <QEAnnotationModel.hpp> 
#include <QMetaProperty>

using namespace std;
namespace {
	inline QString ANN_CLASS_ID() { return QLatin1Literal( "class");}
	inline QString ANN_TABLE_KEY() { return QLatin1Literal( "@QE.ORM.TABLE");}
	inline QString ANN_TEMPORARY_TABLE_KEY() { return QLatin1Literal( "@QE.ORM.TEMPORARY_TABLE");}
	
	inline QString ANN_EXPORT_PARENT_KEY() { return  QLatin1Literal( "@QE.ORM.EXPORT_PARENT");}
	inline QString ANN_PRIMARY_KEY() { return QLatin1Literal( "@QE.ORM.PRIMARY_KEY");}
	inline QString ANN_INDEX() { return QLatin1Literal( "@QE.ORM.INDEX");}
	
	inline QString ANN_ENABLE() { return QLatin1Literal( "@QE.ORM.ENABLE");}
}

QEOrmModelPrivate::QEOrmModelPrivate( const QEAnnotationModel& model, const QMetaObject* metaObject)
{
	parseAnnotations( model, metaObject);
}

QEOrmModelPrivate::QEOrmModelPrivate(const QEOrmModelPrivate &other) = default;

/// @brief It gets the primary keys from annotation.
/// 
/// If there is no explicit primary key, it will use the first
/// 'auto_increment' file as a primary key.
vector<QEOrmColumnDef> QEOrmModelPrivate::parseAnnotationsGetPrimaryKeys( const QEAnnotationModel& model) const 
{
	vector<QEOrmColumnDef> pk;
	QStringList pkPropertyNames = model.annotation( ANN_CLASS_ID(), ANN_PRIMARY_KEY())
		.value( QString()).toString()
		.split( ',', QString::SkipEmptyParts);
	
	if( pkPropertyNames.isEmpty())
	{
		QEOrmColumnDef colDef = findColumnDefIf(
			[]( const QEOrmColumnDef& colDef)
				{ return colDef.isDbAutoIncrement();});

		if( colDef.isValid())
			pkPropertyNames << colDef.propertyName();
	}

	for( QString pkPropName: pkPropertyNames)
	{
		QEOrmColumnDef colDef = findColumnDefIf( 
				[&pkPropName]( const QEOrmColumnDef& colDef)
				{ return pkPropName == colDef.propertyName(); });
		pk.push_back( colDef);
		colDef.setPartOfPrimaryKey( true);
	}
	
	return pk;
}

void QEOrmModelPrivate::parseAnnotations( const QEAnnotationModel &model, 
										  const QMetaObject* metaObj)
{
	// Table
	m_table = model.annotation( ANN_CLASS_ID(), ANN_TABLE_KEY())
		.value( QString( metaObj->className())).toString();
	// Columns			
	const bool exportParents = model.annotation( ANN_CLASS_ID(), ANN_EXPORT_PARENT_KEY())
		.value( false).toBool();
	const int begin = (exportParents) ? 0 : metaObj->propertyOffset();
	for( int i = begin; i < metaObj->propertyCount(); ++i)
	{
		QMetaProperty property = metaObj->property(i);
		const QByteArray propertyName = property.name();
		const bool isEnable = model.annotation( propertyName, ANN_ENABLE())
			.value( true).toBool();
		if( isEnable )
		{
			QEOrmColumnDef colDef( propertyName, property.type(), model);
			m_columns.push_back( colDef);
		}
	}
	
	// Primary keys and no pk
	m_primaryKey = parseAnnotationsGetPrimaryKeys( model);
	// noPrimaryKey = generateNoPrimaryKey();
}


QEOrmColumnDef QEOrmModelPrivate::findColumnDefIf( const std::function<bool(const QEOrmColumnDef&)> predicate) const
{
	QEOrmColumnDef column;
	const auto itr = find_if( begin( m_columns), end( m_columns), predicate);
	if( itr != end( m_columns))
		column = *itr;

	return column;
}


#if 0
vector< QEOrmColumnDef > QEOrmModelPrivate::generateNoPrimaryKey() const
{
	vector< QEOrmColumnDef> noPk;
	for( const ColumnDefBy::value_type &item : columnsByName)
	{
		const QEOrmColumnDef colDef = item.second;
		const auto itr = find_if( begin(primaryKey), end(primaryKey),
								  [colDef]( const QEOrmColumnDef& pkColDef)
								  { return colDef.propertyName() == pkColDef.propertyName(); });
		if( itr == end(primaryKey))
			noPk.push_back( colDef);
	}
	return noPk;
}
#endif

QString QEOrmModelPrivate::table() const noexcept
{ return m_table;}

const vector<QEOrmColumnDef>& QEOrmModelPrivate::columns() const noexcept
{ return m_columns;}

const vector<QEOrmColumnDef>& QEOrmModelPrivate::primaryKey() const noexcept
{ return m_primaryKey;}

const vector<QEOrmForeignDef>& QEOrmModelPrivate::referencesToOne() const noexcept
{ return m_referencesToOne;}

void QEOrmModelPrivate::addReferencesToOne( const QEOrmForeignDef& fkDef)
{
	// Add columns 
	for( QEOrmColumnDef fkColDef : fkDef.foreignKeys())
	{
		fkColDef.setDbAutoIncrement( false);
		fkColDef.setPartOfPrimaryKey( false);
		fkColDef.setDbUnique( false);
		m_columns.push_back( fkColDef);
	}

	// Copy ref
	m_referencesToOne.push_back( fkDef);
}
