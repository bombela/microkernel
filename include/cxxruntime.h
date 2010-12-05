/*
 * cxxruntime.h
 * Copyright © 2010 François-Xavier 'Bombela' Bourlet <bombela@gmail.com>
 *
*/

#pragma once
#ifndef CXXRUNTIME_H
#define CXXRUNTIME_H

namespace cxxruntime {

void initialize();
void finalize();

// A cute helper ;)
struct Run
{
	Run() { initialize(); }
	~Run() { finalize(); }
};

} // namespace cxxruntime

#endif /* CXXRUNTIME_H */
