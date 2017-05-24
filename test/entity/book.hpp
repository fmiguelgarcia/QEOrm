#pragma once
#include <QObject>
#include "entity/chapter.hpp"
#include <qe/entity/OneToManyAdapter.hpp>
#include <vector>

class Book 
	: public QObject
{
	Q_OBJECT 
	Q_CLASSINFO( "class", "@qe.model.name=book")

	Q_PROPERTY( int id MEMBER id)
	Q_CLASSINFO( "id", "@qe.entity.isAutoIncrementable=true @qe.entity.name=bid")
	Q_PROPERTY( QString title MEMBER title)
	Q_CLASSINFO( "title", "@qe.entity.maxLength=256")
	Q_PROPERTY( QString author MEMBER author)
	Q_CLASSINFO( "author", "@qe.entity.maxLength=512")
	Q_PROPERTY( int pages MEMBER pages)
	Q_CLASSINFO( "pages", "@qe.entity.maxLength=512")
	Q_PROPERTY( QByteArray binSignature MEMBER binSignature)
	Q_CLASSINFO( "binSignature", "@qe.entity.isNullable=true")

	Q_PROPERTY( int entityDisable MEMBER entityDisable )
	Q_CLASSINFO( "entityDisable", "@qe.entity.isEnabled=false")

#if QT_VERSION >= QT_VERSION_CHECK( 5, 5, 0)
	Q_PROPERTY( Type type READ type WRITE setType)
	Q_CLASSINFO( "type", "@qe.entity.name=bookType")
#endif

	Q_PROPERTY( QVariantList chapters READ chaptersAdapter WRITE chaptersAdapter)
	Q_CLASSINFO( "chapters", "@qe.entity.mapping.entity=Chapter @qe.entity.mapping.type=OneToMany")
	
	public:
		enum Type {
			Unknown,
			Technical,
			Drama,
			History,
			SelfHelp,
			Fantasy
		};
#if QT_VERSION >= QT_VERSION_CHECK( 5, 5, 0)
		Q_ENUM( Type);
#endif

		int id = 0;
		QString title;
		QString author;
		int pages;
		QByteArray binSignature;
		int entityDisable;

		std::vector<Chapter> chapters;
		qe::entity::OneToManyAdapter< decltype(chapters)> chaptersAdapter{ chapters};
		
		bool operator == (const Book& other) const;
		
		Type type() const;
		void setType( const Type type); 

	private:
		Type m_type = Type::Unknown;
};
