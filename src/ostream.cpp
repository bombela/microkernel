#include <kernel/std/ostream>

namespace kernel {
	namespace std {
		
		ostream& ostream::operator<<(int num)
		{
			int idx = 1;
			int tmp;

			for (;num/idx>10;idx*=10);

			for (;idx >= 1; idx/=10)
			{
				tmp = (num/idx)+'0';
				num -= (num/idx)*idx;
				_streambuf->write(tmp);
			}
			return *this;
		}

	}
}
