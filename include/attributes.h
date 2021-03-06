/*
 * attributes.h
 * Copyright © 2010 François-Xavier 'Bombela' Bourlet <bombela@gmail.com>
 *
*/

#pragma once
#ifndef ATTRIBUTES_H
#define ATTRIBUTES_H

#define PACKED __attribute__((packed))
#define UNUSED __attribute__((unused))
#define ALIGNED(__value) __attribute__((aligned( (__value) )))
#define SECTION(__section) __attribute__ ((section ( (__section) )))
#define NORETURN __attribute__ ((noreturn))
#define INIT_PRIORITY(__value) __attribute__ ((init_priority (__value)))
#define NOINLINE __attribute__ ((noinline))

#endif /* ATTRIBUTES_H */
