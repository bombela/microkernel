#include <kernel/std/ostream>
#include <kernel/std/nbprinter.hpp>

namespace kernel {
	namespace std {
		
		ostream& ostream::operator<<(int num)
		{
			auto printer = nbprinter::mkprinter<int>("0123456789");
			*this << printer(num);
			return *this;
		}

	}
}
