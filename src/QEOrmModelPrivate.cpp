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


QEOrmModelPrivate::QEOrmModelPrivate() = default;
QEOrmModelPrivate::QEOrmModelPrivate(const QEOrmModelPrivate &other) = default;

/// @brief It gets the primary keys from annotation.
/// 
/// If there is no explicit primary key, it will use the first
/// 'auto_increment' file as a primary key.
vector<QEOrmColumnDef> QEOrmModelPrivate::parseAnnotationsGetPrimaryKeys( const QEAnnotationModel* model) const 
{
	vector<QEOrmColumnDef> pk;
	QStringList pkPropertyNames = model->annotation( ANN_CLASS_ID(), ANN_PRIMARY_KEY())
		.value( QString()).toString()
		.split( ',', QString::SkipEmptyParts);
	
	if( pkPropertyNames.isEmpty())
	{
		auto itr = find_if(
			begin( columnsByProperty),
			end( columnsByProperty),
				[]( const QEOrmModelPrivate::ColumnDefBy::value_type& item)
				{ return item.second.isDbAutoIncrement();});
		if( itr != end( columnsByProperty))
			pkPropertyNames << itr->second.propertyName();
	}
	
	for( QString pkPropName: pkPropertyNames)
	{
		auto itr = columnsByProperty.find( pkPropName);
		if( itr != end( columnsByProperty))
			pk.push_back( itr->second);
	}
	
	return pk;
}

void QEOrmModelPrivate::parseAnnotations( const QEAnnotationModel *model, 
										  const QMetaObject* metaObj)
{
	// Table
	table = model->annotation( ANN_CLASS_ID(), ANN_TABLE_KEY())
		.value( QString( metaObj->className())).toString();
	// Columns			
	const bool exportParents = model->annotation( ANN_CLASS_ID(), ANN_EXPORT_PARENT_KEY())
		.value( false).toBool();
	const int begin = (exportParents) ? 0 : metaObj->propertyOffset();
	for( int i = begin; i < metaObj->propertyCount(); ++i)
	{
		QMetaProperty property = metaObj->property(i);
		const QByteArray propertyName = property.name();
		const bool isEnable = model->annotation( propertyName, ANN_ENABLE())
			.value( true).toBool();
		if( isEnable )
		{
			QEOrmColumnDef colDef( propertyName, property.type(), model);
			
			columnsByName.insert(
				make_pair( colDef.dbColumnName(), colDef));
			columnsByProperty.insert(
				make_pair( colDef.propertyName(), colDef));
		}
	}
	
	// Primary keys and no pk
	primaryKey = parseAnnotationsGetPrimaryKeys( model);
	noPrimaryKey = generateNoPrimaryKey();
}

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

