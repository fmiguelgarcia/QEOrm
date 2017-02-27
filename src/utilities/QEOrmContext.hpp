#pragma once
#include <QECommon/QEGlobal.hpp>
#include <stack>

class QObject;

QE_BEGIN_NAMESPACE

using QEOrmContext = std::stack<QObject*>;

class ScopedContext
{
	public:
		ScopedContext( QObject* obj, QEOrmContext & context);
		~ScopedContext();

	private:
		QEOrmContext &m_context;
};

QE_END_NAMESPACE
