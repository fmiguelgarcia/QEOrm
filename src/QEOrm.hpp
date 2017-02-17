#pragma once
#include <QEOrmModel.hpp>
#include <DBDriver/SQLGenerator.hpp>
#include <QECommon/QES11n.hpp>
#include <QSet>
#include <memory>
#include <map>
#include <mutex>

QE_BEGIN_NAMESPACE

class QEOrm 
{
	public:
		static QEOrm& instance();

		void save( const QObject* const source) const;
		void load( const QVariantList pk, QObject* target) const;

	private:
		QEOrm();
		Q_DISABLE_COPY(QEOrm)
		QEOrmModel getModel( const QMetaObject *metaObject) const;
		void checkAndCreateDBTable( const QEOrmModel& model) const;
		
		void insertObjectOnDB(const QObject *source, const QEOrmModel& model) const;
		void updateObjectOnDB(const QObject *source, const QEOrmModel& model) const;
		bool existsObjectOnDB(const QObject *source, const QEOrmModel& model) const;
		QString generateCreateTableIfNotExists( const QEOrmModel& model) const;
		
	private:
		mutable std::mutex m_cachedModelsMtx;
		mutable std::map<const QMetaObject*, QEOrmModel> m_cachedModels;
		mutable std::mutex m_cachedCheckedTablesMtx;
		mutable QSet<QString> m_cachedCheckedTables;
		
		std::unique_ptr<SQLGenerator> m_sqlGenerator;

		// Singleton
		static std::unique_ptr<QEOrm> m_instance;
		static std::once_flag m_onceFlag;
};

QE_END_NAMESPACE
