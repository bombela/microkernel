/*
 * mainconsole.h
 * Copyright © 2010 François-Xavier 'Bombela' Bourlet <bombela@gmail.com>
 *
*/

#pragma once
#ifndef MAINCONSOLE_H
#define MAINCONSOLE_H

#include <kernel/iconsole.h>

namespace kernel {
namespace main_console {

extern IConsole* console;
void init();

} // namespace main_console
} // namespace kernel

#endif /* MAINCONSOLE_H */
