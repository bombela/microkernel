/*
 * cxxruntime.h
 * Copyright © 2010 François-Xavier 'Bombela' Bourlet <bombela@gmail.com>
 *
*/

#pragma once
#ifndef CXXRUNTIME_H
#define CXXRUNTIME_H

namespace cxxruntime {

void call_static_constructors();
void call_registereds_destructors();

} // namespace cxxruntime

#endif /* CXXRUNTIME_H */
