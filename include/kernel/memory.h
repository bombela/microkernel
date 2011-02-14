/*
 * memory.h
 * Copyright © 2011, François-Xavier 'Bombela' Bourlet <bombela@gmail.com>
 *
*/

#pragma once
#ifndef MEMORY_H
#define MEMORY_H

namespace kernel {
namespace memory {

enum class Privilege      { BITS = 2, kernel = 0, userland = 3 };
enum class OperationSize  { BITS = 1, mode16 = 0, mode32 = 1 };

} // namespace memory
} // namespace kernel

#endif /* MEMORY_H */
