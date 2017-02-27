#pragma once
#include <helpers/QEOrmSqlHelper.hpp>
#include <utilities/QEOrmContext.hpp>

QE_BEGIN_NAMESPACE

class SQLGenerator;
class QEOrmLoadHelper
{
	public:
		explicit QEOrmLoadHelper( SQLGenerator* sqlGen);

		void load( const QVariantList pk, const QEOrmModelShd& model,
				QObject* target, const QEOrmContext& context) const;

	private:
		void loadObjectFromRecord( QObject* target, const QEOrmModel& model, 
				QSqlRecord& record) const;

		void loadOneToMany( QObject* target, const QEOrmModelShd& model,
				const QEOrmContext& context ) const;

		QVariantList loadObjectsUsingForeignKey( 
				const QMetaObject* refMetaObjectEntity, QObject* target,
				const QEOrmForeignDef& fkDef, const QEOrmModel &refModel) const;

	public:
		QEOrmSqlHelper sqlHelper;

	private:
		SQLGenerator *m_sqlGenerator;
};

QE_END_NAMESPACE
