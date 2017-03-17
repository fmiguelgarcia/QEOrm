#include <helpers/QEOrmSaveHelper.hpp>
#include <utilities/QEOrmContext.hpp>
#include <QEOrm.hpp>

QE_USE_NAMESPACE
using namespace std;
Q_LOGGING_CATEGORY( QeOrmHelperSave, "com.dmious.qe.orm.helper.save")

QEOrmSaveHelper::QEOrmSaveHelper( SQLGenerator* sqlGen)
	: m_sqlGenerator( sqlGen)
{}

void QEOrmSaveHelper::save(QObject *const source, const QEOrmModelShd& model,
	stack<QObject*> &context) const
{
	if( exists( source, *model))
		update( source, context, *model);
	else
		insert( source, context, *model);

	saveOneToMany( source, context, model);
}

bool QEOrmSaveHelper::exists(const QObject *source, const QEOrmModel &model) const
{
	QVariantList pkValues;
	for( const auto& colDef: model.primaryKeyDef())
	{
		const QVariant value = source->property( colDef->propertyName);
		if( colDef->isDbAutoIncrement && value.toInt() == 0)
			pkValues << QVariant();
		else
			pkValues << value;
	}
	
	QSqlQuery query = sqlHelper.execute( 
		m_sqlGenerator->generateExistsObjectOnDBStmt( source, model),
		pkValues,
		QString("QEOrm cannot check existance of object into db %1: %2"));

	return query.next();
}

void QEOrmSaveHelper::insert(QObject *source, const stack<QObject*>& context, 
		const QEOrmModel &model) const
{
	QSqlQuery query = sqlHelper.execute( 
		source, model, context, 
		m_sqlGenerator->generateInsertObjectStmt( source, model),
		QLatin1Literal("QEOrm cannot insert a new object into database %1: %2"));

	// Update object
	QVariant insertId = query.lastInsertId();
	if( ! insertId.isNull())
	{
		const auto colDef = model.findColumnDef( QEOrmModel::findByAutoIncrement{});
		if( colDef)
			source->setProperty( colDef->propertyName.constData(), insertId);
	}
}

void QEOrmSaveHelper::update(const QObject *source, 
		const QEOrmContext& context, const QEOrmModel &model) const
{
	sqlHelper.execute(
		source, model, context,
		m_sqlGenerator->generateUpdateObjectStmt( source, model),
		QLatin1Literal("QEOrm cannot update an object from database %1: %2"));
}

void QEOrmSaveHelper::saveOneToMany(QObject *const source, 
		QEOrmContext& context, const QEOrmModelShd& model) const
{
	ScopedContext _( source, context);
	for( const auto& colDef : model->columnDefs())
	{
		if( colDef->mappingType == QEOrmColumnDef::MappingType::OneToMany)
		{
			const QByteArray& propertyName = colDef->propertyName;
			const QVariant propertyValue = source->property( propertyName);
			if( ! propertyValue.canConvert<QVariantList>())
				throw runtime_error( 
					QString("QE Orm can only use QVariantList for mapping property %1") 
						.arg( QString(propertyName)).toStdString());

			QVariantList values = propertyValue.toList();
			for( QVariant& value : values)
			{
				QObject *refItem = value.value<QObject*>();
				if( refItem )
				{
					QEOrmModelShd refModel = QEOrm::instance().getModel( refItem->metaObject());
					save( refItem, refModel, context);
				}
			}
			qCDebug( QeOrmHelperSave) 
				<< "Property " << propertyName << " has " 
				<< values.size() << " items.";
		}
	}
}

