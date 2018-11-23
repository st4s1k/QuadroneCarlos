/*
 * main.c
 * 
 * Copyright 2018 Stanislav <stanislav@st4s1k-pc>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 * 
 */
 
#include "quadronecarlos.h"

int main(void)
{
	
	int some_pin = 2;

    set_bits(DDRC, 0xFF);
    
    clear_bits(PORTC, PINC0 | PINC1 | PINC2);

    while(1) {
        clear_bit(PORTC, some_pin);
        _delay_ms(1000);
        set_bit(PORTC, some_pin);
        _delay_ms(1000);
    }

    return 0;
}

