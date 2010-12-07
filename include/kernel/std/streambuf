/* vim: ft=cpp
 * Local Variables:
 * mode:c++
 * End:
 * streambuf
 * Copyright © 2010 François-Xavier 'Bombela' Bourlet <bombela@gmail.com>
 *
*/

#pragma once
#ifndef STREAMBUF_H
#define STREAMBUF_H

namespace kernel {
namespace std {

enum class color {
	black,
	dkgray,
	blue,
	ltblue,
	green,
	ltgreen,
	cyan,
	ltcyan,
	red,
	ltred,
	magenta,
	ltmagenta,
	brown,
	yellow,
	ltgray,
	white,
};

class streambuf
{
	public:
		virtual ~streambuf() {}
		virtual void write(const char*) = 0;
		virtual void write(const char*, size_t) = 0;
		virtual void write(char) = 0;
		virtual void setcolor(color) = 0;
};

} // namespace std
} // namespace kernel

#endif /* STREAMBUF_H */
