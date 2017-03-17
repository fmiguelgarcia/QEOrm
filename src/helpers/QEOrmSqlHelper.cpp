#include <helpers/QEOrmSqlHelper.hpp>
#include <QDate>
#include <QDateTime>
#include <QTime>
#include <QBitArray>
#include <QTextStream>
#include <QDataStream>
#include <QSqlError>

QE_USE_NAMESPACE
using namespace std;
Q_LOGGING_CATEGORY( QeOrmHelperSql, "com.dmious.qe.orm.helper.sql")

void QEOrmSqlHelper::showQuery( QSqlQuery& query) const
{
	if( isShowQueryEnabled )
	{
		const QChar quotationMark('\'');
		const QChar openKey('{');
		const QChar closeKey( '}');

		QString message;
		QTextStream os( &message);
		const QMap<QString, QVariant> boundValues = query.boundValues();

		os << QLatin1Literal( "QE Orm ") << endl
			<< QLatin1Literal( "   + query: ") << query.executedQuery() << endl
			<< QLatin1Literal( "   + parameters: { ");
		for( const QString boundKey : boundValues.keys())
		{
			os << openKey << boundKey << QLatin1Literal(", ");

			const QVariant value = boundValues[boundKey];
			const QVariant::Type valueType = value.type();
			switch( valueType )
			{
				case QMetaType::UnknownType:
					os << QLatin1Literal("null");
					break;
				case QVariant::Type::BitArray:
				{
					QByteArray buffer;
					QDataStream in( buffer);
					in << value.toBitArray();
					os << quotationMark << buffer.toHex()<< quotationMark;
				  	break;
				}	
				case QVariant::Type::ByteArray:
					os << quotationMark << value.toByteArray().toHex() << quotationMark;
					break;
				case QVariant::Type::Date:
					os << value.toDate().toString( Qt::ISODate);
					break;
				case QVariant::Type::Time:
					os << value.toTime().toString( Qt::ISODate);
					break;
				case QVariant::Type::DateTime:
					os << value.toDateTime().toString( Qt::ISODate);
					break;
				case QVariant::Type::String:
					os << quotationMark << value.toString() << quotationMark;
					break;
				default:
			 		os << value.toString();
			}
			os << closeKey;
		}
		os << closeKey << endl;

		qCDebug( QeOrmHelperSql) << message;
	}
}

QSqlQuery QEOrmSqlHelper::execute( QSqlQuery& query, const QString& errorMsg) const
{
	const bool isSuccess = query.exec();
	showQuery( query);

	if( !isSuccess)
	{
		const QSqlError sqlError = query.lastError();
		if( sqlError.type() != QSqlError::NoError )
		{
			const QString msg =  errorMsg
				.arg( sqlError.nativeErrorCode())
				.arg( sqlError.text());
			throw runtime_error( msg.toStdString());
		}
	}
	return query;
}
	
QSqlQuery QEOrmSqlHelper::execute( const QString& stmt,
		const QVariantList& bindValues, const QString& errorMsg) const 
{
	QSqlQuery query;
	query.prepare( stmt);
	for( int i = 0; i < bindValues.size(); ++i)
		query.bindValue( i, bindValues[i]);

	return execute( query, errorMsg);
}

void QEOrmSqlHelper::execute( const QStringList& stmtList, 
		const QString& errorMsg) const
{
	for( const QString& stmt: stmtList)
	{
		QSqlQuery query;
		query.prepare( stmt);
		execute( query, errorMsg);
	}
}
	
QSqlQuery QEOrmSqlHelper::execute( const QObject* object, const QEOrmModel& model,
			const stack<QObject*> &context,  
			const QString& stmt, const QString& errorMsg ) const
{
	// 1. Prepare statement.
	QSqlQuery query;
	query.prepare( stmt);

	// 2. Bind values
	bindNoMappingColumns( query, object, model);
	bindMappingOneToMany( query, model, context);

	// 3. Execute
	return execute( query, errorMsg);
}

void QEOrmSqlHelper::bindNoMappingColumns( QSqlQuery& query, const QObject* o, 
				const QEOrmModel& model) const
{
	for( const auto& colDef: model.columnDefs())
	{
		if( colDef->mappingType == QEOrmColumnDef::MappingType::NoMappingType)
		{
			QVariant value = o->property( colDef->propertyName);
			if( colDef->isDbAutoIncrement && value.toInt() == 0)
				value = QVariant();
			query.bindValue( QString(":%1").arg( colDef->dbColumnName), value);
		}
	}
}

void QEOrmSqlHelper::bindMappingOneToMany( QSqlQuery& query,  
	const QEOrmModel& model, const stack<QObject*>& context ) const
{
	if( !context.empty())
	{
		QObject *contextTopObject = context.top();
		for( const auto& fkDef : model.referencesManyToOneDefs())
		{
			const auto& foreignKeys = fkDef->foreignKeys();
			const auto& refKeys = fkDef->reference()->primaryKeyDef();
			for( uint i = 0; i < foreignKeys.size(); ++i) 
			{
				const QVariant value = contextTopObject->property( 
						refKeys[i]->propertyName); 

				query.bindValue( 
						QString( ":%1").arg( foreignKeys[i]->dbColumnName),
						value);
			}
		}
	}
}

