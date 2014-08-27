/*
# Copyright (C) 2006 Peter Fleury, modified by Manuel Bentele
#
# Without an official permission commercial use is not permitted.
# Forking this project is not permitted.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/

#ifndef UART_H_INCLUDED
#define UART_H_INCLUDED

#include <inttypes.h>

#define UART_SUCCESS    0x00
#define UART_FULL       0x01
#define UART_EMPTY      0x02

extern void uart_init(void);

extern uint8_t uart_putc(uint8_t data);

extern uint8_t uart_puts(const int8_t *s);

extern uint8_t uart_getc(uint8_t *data);

#endif // UART_H_INCLUDED
