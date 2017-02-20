#include "QEOrmColumnDefPrivate.hpp"
#include <QEAnnotation/QEAnnotationModel.hpp>

QE_USE_NAMESPACE 
namespace {
	inline QString ANN_AUTO_INCREMENT() { return QLatin1Literal( "@QE.ORM.AUTO_INCREMENT");}
	inline QString ANN_DB_COLUMN() { return QLatin1Literal( "@QE.ORM.DB_COLUMN");}
	inline QString ANN_MAX_LENGTH() { return QLatin1Literal( "@QE.ORM.MAX_LENGTH");}
	inline QString ANN_NULL() { return QLatin1Literal( "@QE.ORM.NULL");}
	inline QString ANN_UNIQUE() { return QLatin1Literal( "@QE.ORM.UNIQUE");}
	inline QString ANN_DEFAULT() { return QLatin1Literal( "@QE.ORM.DEFAULT");}
}

QEOrmColumnDefPrivate::QEOrmColumnDefPrivate() = default;
QEOrmColumnDefPrivate::QEOrmColumnDefPrivate(const QEOrmColumnDefPrivate &other) = default;


QEOrmColumnDefPrivate::QEOrmColumnDefPrivate( const QByteArray& property, const int type, const QEAnnotationModel* model)
	: propertyName(property), propertyType( type)
{
	dbColumnName = model->annotation( property, ANN_DB_COLUMN())
		.value( property).toString();
	isDbNullable = model->annotation( property, ANN_NULL())
		.value( false).toBool();
	isDbAutoIncrement = model->annotation( property, ANN_AUTO_INCREMENT())
		.value( false).toBool();
	isDbUnique = model->annotation( property, ANN_UNIQUE())
		.value( false).toBool();
	dbMaxLength = model->annotation( property, ANN_MAX_LENGTH())
		.value( 0).toUInt();
		
	if( type == QMetaType::Char 
		|| type == QMetaType::QChar
		|| type == QMetaType::SChar
		|| type == QMetaType::UChar)
		dbMaxLength = 1;
}
