// lundi 03 octobre 2005

#ifndef utils
#define utils

inline std::string IntToString(int x)
{
	std::ostringstream o;
	o << x;
	return o.str();
}

#endif
