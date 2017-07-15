#pragma once
#include <QObject>
#include "entity/chapter.hpp"
#include <qe/entity/OneToManyAdapter.hpp>
#include <vector>

class Book 
	: public QObject
{
	Q_OBJECT 
	Q_PROPERTY( int id MEMBER id)
	Q_PROPERTY( QString title MEMBER title)
	Q_PROPERTY( QString author MEMBER author)
	Q_PROPERTY( int pages MEMBER pages)
	Q_PROPERTY( QByteArray binSignature MEMBER binSignature)
	Q_PROPERTY( int entityDisable MEMBER entityDisable )
	// Q_PROPERTY( QVariantList chapters READ chaptersAdapter WRITE chaptersAdapter)
	Q_PROPERTY( Book::ChapterList chapters MEMBER chapters)
	Q_PROPERTY( QStringList footNotes MEMBER m_footNotes)
	Q_PROPERTY( QByteArray cover MEMBER m_cover)

#if QT_VERSION >= QT_VERSION_CHECK( 5, 5, 0)
	Q_PROPERTY( Type type READ type WRITE setType)
	Q_CLASSINFO( "type", "@qe.entity.name=bookType")
#endif

	// Annotated-entity for serialization
	Q_CLASSINFO( "class", "@qe.model.name=book")
	Q_CLASSINFO( "id", "@qe.entity.isAutoIncrementable=true @qe.entity.name=bid")
	Q_CLASSINFO( "title", "@qe.entity.maxLength=256")
	Q_CLASSINFO( "author", "@qe.entity.maxLength=512")
	Q_CLASSINFO( "pages", "@qe.entity.maxLength=512")
	Q_CLASSINFO( "binSignature", "@qe.entity.isNullable=true")
	Q_CLASSINFO( "entityDisable", "@qe.entity.isEnabled=false")
	Q_CLASSINFO( "chapters", "@qe.entity.mapping.entity=Chapter @qe.entity.mapping.type=OneToMany")
	Q_CLASSINFO( "cover", "@qe.entity.isNullable=true @qe.entity.maxLength=524288")

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

		using ChapterList = std::vector<Chapter>;
		ChapterList chapters;
		// qe::entity::OneToManyAdapter< decltype(chapters)> chaptersAdapter{ chapters};
		
		bool operator == (const Book& other) const;
		
		Type type() const;
		void setType( const Type type);

		QStringList footNotes() const;
		void addFootNote( const QString& footNote);
		void removeFootNote( const QString& footNote);

		QByteArray cover() const ;
		void setCover( const QByteArray& data);

	private:
		Type m_type = Type::Unknown;

		QStringList m_footNotes;
		QByteArray m_cover;
};

Q_DECLARE_METATYPE( Book::ChapterList)
