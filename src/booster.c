/*
# Copyright (C) 2014 Manuel Bentele
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

#include "include/booster.h"
#include "include/config.h"

#include <avr/interrupt.h>

void booster_init(void)
{
    #ifdef USE_BOOSTER_LEDS
        DDRD |= (1 << DDD5); // red LED as output
        DDRD |= (1 << DDD6); // green LED as output
    #endif

    // Enable booster interrupt for short-circuit signals
    EICRA |= (1 << ISC00) | (1 << ISC01); // interrupt for rising edge
    EIMSK |= (1 << INT0);

    DDRB |= (1 << DDB0); // booster on/off as output
}

void booster_start(void)
{
    #ifdef USE_BOOSTER_LEDS
        PORTD &= ~(1 << PD5); // clear red LED
        PORTD |= (1 << PD6); // set green LED
    #endif

    PORTB |= (1 << PB0); // start booster
}

void booster_stop(void)
{
    #ifdef USE_BOOSTER_LEDS
        PORTD &= ~(1 << PD6); // clear green LED
        PORTD |= (1 << PD5); // set red LED
    #endif

    PORTB &= ~(1 << PB0);  // shutdown booster
}

ISR(INT0_vect)
{
    booster_stop();
}
