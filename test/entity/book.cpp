#include "book.hpp"
#include <qe/entity/SequenceContainerRegister.hpp>


QE_REGISTER_SEQUENCE_CONTAINER(
	Book::ChapterList,
	Chapter)


bool Book::operator == (const Book& other) const
{
	bool equality =
		id == other.id
		&& title == other.title
		&& author == other.author
		&& pages == other.pages
		&& binSignature == other.binSignature
#if QT_VERSION >= QT_VERSION_CHECK( 5, 5, 0)
		&& m_type == other.m_type
#endif
		// entityDisable is not stored/loaded from db.
		// && entityDisable == other.entityDisable
		&& chapters == other.chapters;

	if( equality)
	{
		QStringList thisFootNotes = m_footNotes;
		QStringList otherFootNotes = other.m_footNotes;
		thisFootNotes.sort();
		otherFootNotes.sort();

		equality = thisFootNotes == otherFootNotes;
	}

	return equality;
}

void Book::setType(const Book::Type type)
{
	m_type = type;
}

Book::Type Book::type() const
{
	return m_type;
}

QStringList Book::footNotes() const
{ return m_footNotes; }

void Book::addFootNote( const QString& footNote)
{
	m_footNotes << footNote;
}

void Book::removeFootNote( const QString& footNote)
{
	m_footNotes.removeAll( footNote);
}

QByteArray Book::cover() const
{ return m_cover; }

void Book::setCover( const QByteArray& data)
{ m_cover = data; }
