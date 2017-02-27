#include <utilities/QEOrmContext.hpp>

QE_USE_NAMESPACE
using namespace std;

ScopedContext::ScopedContext( QObject* obj, QEOrmContext & context)
	: m_context( context)
{
	context.push( obj);
}

ScopedContext::~ScopedContext()
{
	m_context.pop();
}

