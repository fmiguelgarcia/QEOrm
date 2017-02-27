#pragma once
#include <QEOrmModel.hpp>
#include <QSqlQuery>
#include <QVariantList>
#include <stack>

QE_BEGIN_NAMESPACE

/// @brief It is a helper to execute SQL statements in @c QEOrm.
class QEOrmSqlHelper
{
	public:
		/// @brief It executes the @p query SQL query. If execution fails, it
		/// throws a @c std::runtime_error exception using the message @p
		/// errorMsg.
		QSqlQuery execute( QSqlQuery& query, const QString& errorMsg) const;
	
		/// @brief It binds the @p bindValues by position before execute the
		/// statement @p stmt.
		QSqlQuery execute( const QString& stmt,
				const QVariantList& bindValues, const QString& errorMsg) const;
	
		/// @brief It binds values from @p object and @p context and executes
		/// the statement @p stmt.	
		QSqlQuery execute( const QObject* object, const QEOrmModel& model,
			const std::stack<QObject*> &context,  const QString& stmt, 
			const QString& errorMsg ) const;

		/// @brief It executes a list of statements, and throws a @c
		/// runtimer_error if any one fails.
		void execute( const QStringList& stmtList, const QString& errorMsg) const;

	private:
		void showQuery( QSqlQuery& query) const;

		void bindNoMappingColumns( QSqlQuery& query, const QObject* o, 
				const QEOrmModel& model) const;

		void bindMappingOneToMany( QSqlQuery& query, const QEOrmModel& model, 
				const std::stack<QObject*>& context ) const;

	public:
		bool isShowQueryEnabled = false;
};

QE_END_NAMESPACE
