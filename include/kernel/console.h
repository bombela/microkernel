/*
 * console.h
 * Copyright © 2010 François-Xavier 'Bombela' Bourlet <bombela@gmail.com>
 *
*/

#pragma once
#ifndef CONSOLE_H
#define CONSOLE_H

namespace kernel {

class Console
{
	public:
		enum class Color {
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

		struct Attr {
			Color fgcolor;
			Color bgcolor;
			bool  blink;

			constexpr Attr(Color fgcolor = Color::ltgray,
					Color bgcolor = Color::black,
					bool blink = false
				): fgcolor(fgcolor), bgcolor(bgcolor), blink(blink) {}
		};

		virtual ~Console() {}
		virtual void write(const char*) = 0;
		virtual void write(char) = 0;
};

extern Console* main_console;
void main_console_init();

} // namespace kernel

#endif /* CONSOLE_H */
