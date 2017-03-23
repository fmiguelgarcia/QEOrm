#include "entity/chapter.hpp"

namespace {
	static const int ChapterPtrTypeId = qRegisterMetaType<Chapter*>();
}

Chapter::Chapter( QObject* parent, const QString& title) noexcept
	:QObject( parent), title( title)
{}

Chapter::Chapter( Chapter&& other) noexcept
	: QObject( nullptr),
	id( std::move( other.id)),
	title( std::move( other.title)),
	text( std::move( other.text))
{
	setObjectName( other.objectName());
}

Chapter::Chapter( const Chapter& other) noexcept
	: QObject( nullptr),
	id( other.id),
	title( other.title),
	text( other.text)
{
	setObjectName( other.objectName());
}

Chapter& Chapter::operator=( const Chapter& other)
{
	id = other.id;
	title = other.title;
	text = other.text;
	setObjectName( other.objectName());
	return *this;
}

bool Chapter::operator==( const Chapter& other) const
{
	const QString oName = objectName();
	const QString otherName = other.objectName();
	
	return id == other.id
		&& title == other.title
		&& text == other.text
		&& oName == otherName;
}