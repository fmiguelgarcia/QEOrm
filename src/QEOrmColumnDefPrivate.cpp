#include "QEOrmColumnDefPrivate.hpp"
#include <QEAnnotation/QEAnnotationModel.hpp>
#include <QMetaMethod>
#include <QMetaObject>
#include <QDebug>

QE_USE_NAMESPACE 
using namespace std;

namespace {
	inline QString ANN_AUTO_INCREMENT() { return QLatin1Literal( "@QE.ORM.AUTO_INCREMENT");}
	inline QString ANN_DB_COLUMN() { return QLatin1Literal( "@QE.ORM.DB_COLUMN");}
	inline QString ANN_MAX_LENGTH() { return QLatin1Literal( "@QE.ORM.MAX_LENGTH");}
	inline QString ANN_NULL() { return QLatin1Literal( "@QE.ORM.NULL");}
	inline QString ANN_UNIQUE() { return QLatin1Literal( "@QE.ORM.UNIQUE");}
	inline QString ANN_DEFAULT() { return QLatin1Literal( "@QE.ORM.DEFAULT");}
	
	
	inline QString ANN_MAPPING_TYPE() { return QLatin1Literal( "@QE.ORM.MAPPING.TYPE");}
	inline QString ANN_MAPPING_ENTITY() { return QLatin1Literal( "@QE.ORM.MAPPING.ENTITY");}
}

QEOrmColumnDefPrivate::QEOrmColumnDefPrivate() = default;
QEOrmColumnDefPrivate::QEOrmColumnDefPrivate(const QEOrmColumnDefPrivate &other) = default;

QEOrmColumnDefPrivate::QEOrmColumnDefPrivate( const QByteArray& property, const int type, const QEAnnotationModel &model)
	: propertyName(property), propertyType( type)
{
	dbColumnName = model.annotation( property, ANN_DB_COLUMN())
		.value( property).toString();
	isDbNullable = model.annotation( property, ANN_NULL())
		.value( false).toBool();
	isDbAutoIncrement = model.annotation( property, ANN_AUTO_INCREMENT())
		.value( false).toBool();
	isDbUnique = model.annotation( property, ANN_UNIQUE())
		.value( false).toBool();
	dbMaxLength = model.annotation( property, ANN_MAX_LENGTH())
        .value( 0).toUInt();
		
	// Mapping
	try {
		decodeMapping( model);
	} catch ( std::runtime_error &error){
		qWarning() << error.what();
		mappingType = MappingType::NoMappingType;
	}

	if( type == QMetaType::Char 
		|| type == QMetaType::QChar
		|| type == QMetaType::SChar
		|| type == QMetaType::UChar)
		dbMaxLength = 1;
}

void QEOrmColumnDefPrivate::decodeMapping(const QEAnnotationModel &model)
{
	bool enumOk;
	QMetaEnum meMappingType = QMetaEnum::fromType< QEOrmColumnDefPrivate::MappingType>();
	const QString mappingTypeStr = model.annotation( propertyName, ANN_MAPPING_TYPE())
        .value( QLatin1Literal("NoMappingType")).toString();
	mappingType = static_cast<MappingType>( meMappingType.keyToValue( mappingTypeStr.toLocal8Bit().constData(), &enumOk)); 
	if( !enumOk)
		throw runtime_error( QString("QE Orm cannot decode mapping type '%1' on property '%2'").arg( mappingTypeStr).arg( QString( propertyName)).toStdString());
	
	if( mappingType != MappingType::NoMappingType)
	{
        const QString mappingEntityName = model.annotation( propertyName, ANN_MAPPING_ENTITY()).value().toString();
		if( mappingEntityName.isEmpty())
			throw runtime_error( QString("QE Orm requires no empty mapping entity on property '%1'").arg( QString(propertyName)).toStdString());
		
		// Check metadata is avaible.
		const QString classPointer = mappingEntityName + QChar('*');
		const int typeId = QMetaType::type( classPointer.toLocal8Bit());
		if( typeId == QMetaType::UnknownType)
			throw runtime_error( QString("QE Orm cannot map entity of type '%1'").arg( mappingEntityName).toStdString());
		
		mappingEntity = QMetaType::metaObjectForType( typeId);
	}
}
