#include "book.hpp"

bool Book::operator == (const Book& other) const
{
	return id == other.id
		&& title == other.title
		&& author == other.author
		&& pages == other.pages
		&& binSignature == other.binSignature
		// entityDisable is not stored/loaded from db.
		// && entityDisable == other.entityDisable
		&& chapters == other.chapters;
}