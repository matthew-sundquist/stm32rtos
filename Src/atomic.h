/*
 * atomic.h
 *
 *  Created on: Jun 6, 2026
 *      Author: Matthew Sundquist
 */

#ifndef ATOMIC_H_
#define ATOMIC_H_

// this relies on the assumption that this kernel is only run on a single core

#define atomic_load_8(x) (*(volatile uint8_t *)(x))
#define atomic_load_16(x) (*(volatile uint16_t *)(x))
#define atomic_load_32(x) (*(volatile uint32_t *)(x))

#define atomic_store_8(x, y) (*(volatile uint8_t *)(x) = (y))
#define atomic_store_16(x, y) (*(volatile uint16_t *)(x) = (y))
#define atomic_store_32(x, y) (*(volatile uint32_t *)(x) = (y))

#endif /* ATOMIC_H_ */
