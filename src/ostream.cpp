#include <kernel/std/ostream>
#include <kernel/std/nbprinter.hpp>

namespace kernel {
namespace std {
	
namespace {

template <typename T>
ostream& printnb(ostream& os, T num)
{
	switch (os.flags.getbase())
	{
		case numbase::bin:
			{
				auto printer = nbprinter::mkprinter<T>("01");
				os << printer(num);
				break;
			}
		case numbase::oct:
			{
				auto printer = nbprinter::mkprinter<T>("012345678");
				os << printer(num);
				break;
			}
		case numbase::hex:
			if (os.flags.isuppercase()) {
				auto printer = nbprinter::mkprinter<T>("0123456789ABCDEF");
				os << printer(num);
			} else {
				auto printer = nbprinter::mkprinter<T>("0123456789abcdef");
				os << printer(num);
			}
			break;
		case numbase::dec:
		default:
			{
				auto printer = nbprinter::mkprinter<T>("0123456789");
				os << printer(num);
			}
	}
	return os;
}

} // namespace 

ostream& ostream::operator<<(int v) { return printnb(*this, v); }
ostream& ostream::operator<<(unsigned int v) { return printnb(*this, v); }
ostream& ostream::operator<<(long int v) { return printnb(*this, v); }
ostream& ostream::operator<<(unsigned long int v) { return printnb(*this, v); }

} // namespace std
} // namespace kernel
