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

#endif /* ATTRIBUTES_H */
