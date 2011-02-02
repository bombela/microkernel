/*
 * ostream.cpp
 * Copyright © 2010 François-Xavier 'Bombela' Bourlet <bombela@gmail.com>
 *
*/

#include <kernel/std/ostream>
#include <kernel/std/nbprinter>

namespace std {
namespace kernel_std {
	
namespace {

template <typename T, numbase HUMAN = numbase::dec>
ostream& printnb(ostream& os, T num)
{
	numbase base = os.flags.getbase();
	switch (base == numbase::human ? HUMAN : base)
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
			os << "0x";
			if (os.flags.isuppercase()) {
				auto print = nbprinter::mkprinter<T>("0123456789ABCDEF");
				os << print(num);
			} else {
				auto print = nbprinter::mkprinter<T>("0123456789abcdef");
				os << print(num);
			}
			break;
		case numbase::dec:
		case numbase::human: // to prevent warning
		case numbase::numbase_max: // to prevent warning
		default:
			{
				auto print = nbprinter::mkprinter<T>("0123456789");
				os << print(num);
			}
	}
	return os;
}

} // namespace 

ostream& ostream::operator<<(short v) { return printnb(*this, v); }
//ostream& ostream::operator<<(unsigned short v) { return printnb(*this, v); }
ostream& ostream::operator<<(int v) { return printnb(*this, v); }
ostream& ostream::operator<<(unsigned int v) { return printnb(*this, v); }
ostream& ostream::operator<<(long int v) { return printnb(*this, v); }
ostream& ostream::operator<<(unsigned long int v) { return printnb(*this, v); }
ostream& ostream::operator<<(const void* ptr) {
	return printnb<uintptr_t, numbase::hex>(*this,
			reinterpret_cast<uintptr_t>(ptr));
}

namespace {

const char* const boolalpha_lowercase[] = { "false", "true" };
const char* const boolalpha_uppercase[]  = { "FALSE", "TRUE" };

} // namespace 

ostream& ostream::operator<<(bool b) {
	if (flags.isboolalpha())
		*this << (flags.isuppercase() ?
				boolalpha_uppercase : boolalpha_lowercase)[b ? 1 : 0];
	else
			*this << static_cast<int>(b);
	return *this;
}

} // namespace kernel_std
} // namespace std
