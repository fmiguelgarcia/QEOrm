#pragma once
#include <helpers/QEOrmSqlHelper.hpp>

QE_BEGIN_NAMESPACE

class SQLGenerator;
class QEOrmFindHelper
{
	public:
		explicit QEOrmFindHelper( 
				const QMetaObject* mo,
				SQLGenerator* sqlGen);

		QSqlQuery findEqualProperty( const std::map<QString, QVariant>& properties) const;

	private:
		const QMetaObject* m_mo;
		SQLGenerator *m_sqlGenerator;	
};

	
