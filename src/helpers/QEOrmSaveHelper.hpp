#pragma once
#include <helpers/QEOrmSqlHelper.hpp>
#include <utilities/QEOrmContext.hpp>

QE_BEGIN_NAMESPACE

class SQLGenerator;
class QEOrmSaveHelper
{
	public:
		explicit QEOrmSaveHelper( SQLGenerator* sqlGen);

		void save(QObject *const source, const QEOrmModelShd &model, 
				QEOrmContext& context) const;

	private:
		bool exists(const QObject *source, const QEOrmModel &model) const;
		void insert(QObject *source, const QEOrmContext& context, 
				const QEOrmModel &model) const;
		void update(const QObject *source, const QEOrmContext& context, 
				const QEOrmModel &model) const;

		void saveOneToMany(QObject *const source, QEOrmContext& context, 
				const QEOrmModelShd& model) const;

	public:
		QEOrmSqlHelper sqlHelper;

	private:
		SQLGenerator *m_sqlGenerator;
};

QE_END_NAMESPACE
