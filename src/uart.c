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

#include <avr/interrupt.h>

#include "include/uart.h"
#include "include/config.h"

#if ((RX_BUFFER_SIZE % 2) != 0)
    #error RX_BUFFER_SIZE must be 2^n
#elif ((TX_BUFFER_SIZE % 2) != 0)
    #error TX_BUFFER_SIZE must be 2^n
#endif

#define RX_BUFFER_MASK (RX_BUFFER_SIZE - 1)
#define TX_BUFFER_MASK (TX_BUFFER_SIZE - 1)

static volatile uint8_t RX_BUFFER[RX_BUFFER_SIZE];
static volatile uint8_t TX_BUFFER[TX_BUFFER_SIZE];
static volatile uint8_t RX_Head;
static volatile uint8_t RX_Tail;
static volatile uint8_t TX_Head;
static volatile uint8_t TX_Tail;

void uart_init(void)
{
    RX_Head = 0;
    RX_Tail = 0;
    TX_Head = 0;
    TX_Tail = 0;

    // Calculate UBRRn
    #ifdef USE_U2X
        #define UBRRn (F_CPU / (8 * BAUD_RATE)) - 1
        UCSR0A |= (1 << U2X0);
    #else
        #define UBRRn (F_CPU / (16 * BAUD_RATE)) - 1
    #endif

    // Set baud rate
    UBRR0H = (uint8_t)((UBRRn) >> 8);
    UBRR0L = (uint8_t)(UBRRn);

    // Enable receiver and transmitter
    UCSR0B |= (1 << RXEN0) | (1 << TXEN0);

    // Set frame format: 8N1
    UCSR0C |= (1 << UCSZ00) | (1 << UCSZ01);

    // Enable interrupt to receive data
    UCSR0B |= (1 << RXCIE0);
}

uint8_t uart_putc(uint8_t data)
{
    uint8_t tmpHead;

    tmpHead = ((TX_Head + 1) & TX_BUFFER_MASK);

    if(tmpHead == TX_Tail)
    {
        // no space in buffer
        return UART_FULL;
    }

    TX_BUFFER[tmpHead] = data;
    TX_Head = tmpHead;

    // enable UDRE interrupt
    UCSR0B |= (1 << UDRIE0);
    return UART_SUCCESS;
}

uint8_t uart_puts(const int8_t *s)
{
    uint8_t status = UART_SUCCESS;

    while(*s)
    {
        if(uart_putc(*s++) != UART_SUCCESS)
        {
            status = UART_FULL;
        }
        else
        {
            status = UART_SUCCESS;
        }
    }

    return status;
}

uint8_t uart_getc(uint8_t *data)
{
    uint8_t tmpTail;

    if(RX_Head == RX_Tail)
    {
        return UART_EMPTY; // no data available
    }

    // calculate and store buffer index
    tmpTail = ((RX_Tail + 1) & RX_BUFFER_MASK);
    RX_Tail = tmpTail;

    // get data from receive buffer
    *data = RX_BUFFER[tmpTail];
    return UART_SUCCESS;
}

ISR(USART_RX_vect)
{
    uint8_t tmpHead;
    uint8_t data;

    // read UART status register and UART data register
    data = UDR0;

    // calculate buffer index
    tmpHead = ((RX_Head + 1) & RX_BUFFER_MASK);

    if(tmpHead == RX_Tail)
    {
        // error: receive buffer overflow
    }
    else
    {
        // store new index
        RX_Head = tmpHead;
        // store received data in buffer
        RX_BUFFER[tmpHead] = data;
    }
}

ISR(USART_UDRE_vect)
{
    uint8_t tmpTail;

    if(TX_Head != TX_Tail)
    {
        // calculate and store new buffer index
        tmpTail = ((TX_Tail + 1) & TX_BUFFER_MASK);
        TX_Tail = tmpTail;
        // get one byte from buffer and write it to UART
        UDR0 = TX_BUFFER[tmpTail];  // start transmission
    }
    else
    {
        // TX buffer empty; disable USART_TX_INTTERUPT interrupt
        UCSR0B &= ~(1 << UDRIE0);
    }
}
