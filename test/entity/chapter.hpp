#pragma once
#include <QObject>

class Chapter
	: public QObject
{
	Q_OBJECT 
	Q_CLASSINFO( "class", "@qe.entity.isParentExported=true @qe.entity.primaryKey=id") 

	Q_PROPERTY( int id MEMBER id)
	Q_PROPERTY( QString title MEMBER title)
	Q_CLASSINFO( "title", "@qe.entity.maxLength=256")
	Q_PROPERTY( QString text MEMBER text)
	Q_CLASSINFO( "text", "@qe.entity.isNullable=true")

	public:
		Q_INVOKABLE explicit Chapter( QObject* parent = nullptr, 
				const QString& title = QString()) noexcept;
		Chapter( Chapter&& other) noexcept;
		Chapter( const Chapter& other) noexcept;
		
		Chapter& operator=( const Chapter& other);
		bool operator==( const Chapter& other) const;

	public:
		int id;
		QString title;
		QString text;
};
Q_DECLARE_METATYPE( Chapter*)
