#include "book.hpp"

bool Book::operator == (const Book& other) const
{
	return id == other.id
		&& title == other.title
		&& author == other.author
		&& pages == other.pages
		&& binSignature == other.binSignature
		&& m_type == other.m_type
		// entityDisable is not stored/loaded from db.
		// && entityDisable == other.entityDisable
		&& chapters == other.chapters;
}

void Book::setType(const Book::Type type)
{
	m_type = type;
}

Book::Type Book::type() const
{
	return m_type;
}
