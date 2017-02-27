#include <helpers/QEOrmFindHelper.hpp>
#include <QEOrm.hpp>

QEOrmFindHelper::QEOrmFindHelper( const QMetaObject* mo, SQLGenerator* sqlGen)
	: m_mo(mo), m_sqlGenerator( sqlGen)
{}

QSqlQuery QEOrmFindHelper::findEqualProperty( const std::map<QString, QVariant>& properties) const
{
	QSqlQuery query;
	QEOrmModelShd model = QEOrm::instance().getModel( m_mo);
	if( model)
	{
		QEOrmModel::QEOrmColumnDefList colList;
		QVariantList values;
		for( const auto& propItem: properties)
		{
			QEOrmColumnDefShd colDef = model->findColumnDef( QEOrmModel::findByPropertyName{ propItem.first });
			if( colDef )
			{
				colList.push_back( colDef);
				values.push_back( propItem.second);
			}
		}
		const QString sqlStmt = m_sqlGenerator->selectionUsingProperties( colList, *model);
			
		QEOrmSqlHelper helper;
		helper.isShowQueryEnabled = true;
		query = helper.execute( sqlStmt, values, 
				QString("QE Orm Find Helper cannot execute query, error %1: %2")); 
	}
	return query;
}

