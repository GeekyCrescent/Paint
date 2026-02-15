/* 
 * MDFourier for DOS
 * Copyright (C)2024 - Based on X68000 version by Artemio Urbina
 *
 * This file is part of MDFourier
 *
 * MDFourier is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * MDFourier is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with MDFourier; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
 
#include <conio.h>
#include <dos.h>

// returns 1 on enter/space, -1 on ESC, 0 otherwise
int read_input()
{
	if (kbhit()) {
		int ch = getch();
		if (ch == 0 || ch == 0xE0) {
			// Extended key, consume the second byte
			getch();
			return 0;
		}
		if (ch == 27) {  // ESC
			return -1;
		}
		if (ch == 13 || ch == ' ') {  // Enter or Space
			return 1;
		}
	}
	return 0;
}
