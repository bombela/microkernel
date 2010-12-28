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
				auto print = nbprinter::mkprinter<T>("01");
				os << print(num);
				break;
			}
		case numbase::oct:
			{
				auto print = nbprinter::mkprinter<T>("012345678");
				os << print(num);
				break;
			}
		case numbase::hex:
			if (os.flags.isuppercase()) {
				auto print = nbprinter::mkprinter<T>("0123456789ABCDEF");
				os << print(num);
			} else {
				auto print = nbprinter::mkprinter<T>("0123456789abcdef");
				os << print(num);
			}
			break;
		case numbase::dec:
		default:
			{
				auto print = nbprinter::mkprinter<T>("0123456789");
				os << print(num);
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
