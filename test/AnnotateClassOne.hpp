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

#pragma once
#include <QObject>
#include <QString>
#include <QDateTime>
#include <QVariant>
#include "FKClass.hpp"
#include <iterator>
#include <QEOrmContainerWrapper.hpp>

class AnnotateClassOne
	: public QObject
{
	Q_OBJECT
	Q_PROPERTY( int id MEMBER m_id)
	Q_PROPERTY( QString user MEMBER m_user)
	// Q_PROPERTY( QDateTime begin MEMBER m_begin)
	Q_PROPERTY( QDateTime begin READ begin WRITE setBegin )
	// Q_PROPERTY( QDateTime end MEMBER m_end)
	Q_PROPERTY( QDateTime end READ end WRITE setEnd)
	
	// Q_PROPERTY( std::vector<FKClass> fkClass READ fkClass WRITE setFkClass)
	Q_PROPERTY( QVariantList fkClass READ m_fkClassCW WRITE m_fkClassCW)

	Q_CLASSINFO( "id", "@QE.ORM.AUTO_INCREMENT=true")
	Q_CLASSINFO( "user", "@QE.ORM.NULL=false @QE.ORM.MAX_LENGTH=256")
	Q_CLASSINFO( "fkClass", "@QE.ORM.MAPPING.TYPE=OneToMany @QE.ORM.MAPPING.ENTITY=FKClass @QE.ORM.MAPPING.FETCH=Lazy")
	Q_CLASSINFO( "fkClassWrapper", "QE.ORM.ENABLE=false")

	
	public:
		explicit AnnotateClassOne( QObject* parent = nullptr);
		
		QDateTime begin() const;
		QDateTime end() const;
		void setBegin( const QDateTime& d);
		void setEnd( const QDateTime& d);
		
		bool operator == ( const AnnotateClassOne& other) const;
		
		std::vector<FKClass> fkClass() const;
		void setFkClass( const std::vector<FKClass>& );

		QVariantList wrapFkClass() const;
		void setWrapFkClass( const QVariantList& value);
		
	public:
		int m_id;
		QString m_user;
		QDateTime m_begin;
		QDateTime m_end;
		std::vector<FKClass> m_fkClass;
		std::vector<FKClass> m_fkClass2;
		
		//QE_ORM_MAP_ONE_TO_MANY( fk2, m_fkClass2)

		QEOrmContainerWrapper< decltype(m_fkClass) > m_fkClassCW {m_fkClass};
};
