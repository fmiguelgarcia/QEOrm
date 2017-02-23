#include <QEOrmForeignDef.hpp>
#include <QEOrmModel.hpp>

QE_USE_NAMESPACE
using namespace std;

namespace {
	QString generateValidFKName( const QString& pkColName, QEOrmModel& targetModel, const QEOrmModel& reference)
	{
		QString fkColName = pkColName;
		QEOrmColumnDef colDefFound = targetModel.findColumnByName( fkColName);
		if( colDefFound.isValid())
		{
			uint idx = 0;
			fkColName = QString( "%1%2").arg( reference.table()).arg( pkColName);
			colDefFound = targetModel.findColumnByName( fkColName);
			while( colDefFound.isValid())
			{
				fkColName = QString( "%1%2%3")
					.arg( reference.table()).arg( pkColName)
					.arg( ++idx);
				colDefFound = targetModel.findColumnByName( fkColName);
			}
		}
		return fkColName;
	}
}

QEOrmForeignDef::QEOrmForeignDef( 
		QEOrmModel& targetModel,
		const QEOrmModel& reference, 
		const vector<QEOrmColumnDef>& primaryKey)
	: m_reference( reference), m_refPrimaryKey( primaryKey)
{
	for( QEOrmColumnDef pkColDef : primaryKey)
	{
		const QString fkColName = generateValidFKName( pkColDef.dbColumnName(), targetModel, reference);
		pkColDef.setDbColumnName( fkColName);
		m_foreignKey.push_back( pkColDef);
	}
}

	
const vector<QEOrmColumnDef>& QEOrmForeignDef::foreignKeys() const noexcept
{ return m_foreignKey;}

const vector<QEOrmColumnDef>& QEOrmForeignDef::referenceKeys() const noexcept
{ return m_refPrimaryKey;}

#if 0
uint QEOrmForeignDef::size() const noexcept
{ return m_foreignKey.size(); }
	
QEOrmColumnDef QEOrmForeignDef::foreignKey( const uint idx) const
{ return m_foreignKey[idx]; }

QEOrmColumnDef QEOrmForeignDef::referenceKey( const uint idx ) const
{ return m_refPrimaryKey[idx]; }
#endif

QEOrmModel QEOrmForeignDef::reference() const
{ return m_reference; }


