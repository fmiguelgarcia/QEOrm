#include <helpers/QEOrmLoadHelper.hpp>
#include <utilities/QEOrmContext.hpp>
#include <DBDriver/SQLGenerator.hpp>
#include <QEOrm.hpp>
#include <QMetaObject>
#include <QSqlRecord>
#include <QSqlField>

QE_USE_NAMESPACE
using namespace std;

QEOrmLoadHelper::QEOrmLoadHelper( SQLGenerator* sqlGen)
	: m_sqlGenerator( sqlGen)
{}

void QEOrmLoadHelper::load( const QVariantList pk, const QEOrmModelShd& model,
	QObject* target, const QEOrmContext& context) const
{
	QSqlQuery query = sqlHelper.execute( 
		m_sqlGenerator->selectionUsingPrimaryKey( pk, *model),
		pk, QString( "QEOrm cannot load object from database %1: %2"));
	
	if( !query.next())
		throw runtime_error( "QEOrm cannot load object");

	QSqlRecord record = query.record();
	loadObjectFromRecord( target, *model, record);
	loadOneToMany( target, model, context);
}

void QEOrmLoadHelper::load( QObject* target, QSqlQuery& query)
{
	auto model = QEOrm::instance().getModel( target->metaObject());
	QSqlRecord record = query.record();
	loadObjectFromRecord( target, *model, record);
}

void QEOrmLoadHelper::loadObjectFromRecord( QObject* target, 
	const QEOrmModel& model, QSqlRecord& record) 
{
	for( int i = 0; i < record.count(); ++i)
	{
		const QSqlField field = record.field( i);
		const auto colDef = model.findColumnDef( 
				QEOrmModel::findByColumnName{ field.name()});
		if( colDef )
			target->setProperty( colDef->propertyName.constData(), 
							 field.value());
	}
}

void QEOrmLoadHelper::loadOneToMany( QObject* target, const QEOrmModelShd& model,
		const QEOrmContext& context ) const 
{
	// ScopeStackedContext _( target, context);
	for( const QEOrmColumnDefShd& colDef : model->columnDefs())
	{
		if( colDef->mappingType == QEOrmColumnDef::MappingType::OneToMany)
		{
			QEOrmModelShd manyModel = QEOrm::instance().getModel( colDef->mappingEntity);
			auto fkDef = manyModel->findForeignTo( model);
			if( fkDef )
			{
				QVariantList wrapperList = loadObjectsUsingForeignKey(
					  colDef->mappingEntity, target, *fkDef, *manyModel); 
				
				const QByteArray& propertyName = colDef->propertyName;
				target->setProperty( propertyName, wrapperList);
			}
		}
	}
}

QVariantList QEOrmLoadHelper::loadObjectsUsingForeignKey(
	const QMetaObject* refMetaObjectEntity,
	QObject* target,
	const QEOrmForeignDef& fkDef,
	const QEOrmModel &refModel) const
{
	QVariantList list;

	// Cursor over many objects
	stack<QObject*> emptyContext;
	const QString manySelection = m_sqlGenerator->selectionUsingForeignKey( 
		fkDef, refModel);

	QSqlQuery manyQuery = sqlHelper.execute( target, refModel, emptyContext, 
		manySelection, 
		QString( "QE Orm cannot load one to many relation for entity '%1', error %2: %3")
			.arg( target->metaObject()->className()));

	QEOrmModelShd model = QEOrm::instance().getModel( refMetaObjectEntity);
	while( manyQuery.next())
	{
		// Create object.
		QObject *refObj = refMetaObjectEntity->newInstance( Q_ARG( QObject*, target));

		QSqlRecord record = manyQuery.record();
		loadObjectFromRecord( refObj, *model, record);
		list.push_back( QVariant::fromValue(refObj));
	}

	return list;
}





