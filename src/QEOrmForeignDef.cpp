#include <QEOrmForeignDef.hpp>
#include <QEOrmModel.hpp>

QE_USE_NAMESPACE
using namespace std;

QEOrmForeignDef::QEOrmForeignDef( 
		const QByteArray& propertyName,
		const QEOrmModelShd& reference )
	: m_propertyName( propertyName), m_reference( reference)
{
	for( const auto& pkColDef : reference->primaryKeyDef())
	{
		QEOrmColumnDefShd fk = make_shared<QEOrmColumnDef>(
				pkColDef->propertyName,
				pkColDef->propertyType,
				pkColDef->dbColumnName,
				pkColDef->dbMaxLength);

		m_foreignKey.push_back( fk);
	}
}
	
const QEOrmForeignDef::QEOrmColumnDefList & QEOrmForeignDef::foreignKeys() const noexcept
{ return m_foreignKey;}

const QByteArray& QEOrmForeignDef::propertyName() const noexcept
{ return m_propertyName;}
	 
shared_ptr<QEOrmModel> QEOrmForeignDef::reference() const noexcept
{ return m_reference; }

