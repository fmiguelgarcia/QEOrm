#include <qe/orm/QEOrm.hpp>
using namespace qe::orm;

int main(int argc, char** argv)
{
	const QEOrm& orm = QEOrm::instance();
	(void)(orm);
	return 0;
}
