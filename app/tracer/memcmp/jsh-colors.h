/* This file is part of jsh.
 * 
 * jsh: A basic UNIX shell implementation in C
 * Copyright (C) 2014 Jo Van Bulck <jo.vanbulck@student.kuleuven.be>
 *
 * jsh is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * jsh is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with jsh.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef JSH_COLORS_H_INCLUDED
#define JSH_COLORS_H_INCLUDED

// ANSI escape foreground color codes (see https://en.wikipedia.org/wiki/ANSI_escape_code)
#define BLACK_FG            "\033[30m"
#define RED_FG              "\033[31m"
#define GREEN_FG            "\033[32m"
#define YELLOW_FG           "\033[33m"
#define BLUE_FG             "\033[34m"
#define MAGENTA_FG          "\033[35m"
#define CYAN_FG             "\033[36m"
#define WHITE_FG            "\033[37m"
#define RESET_FG            "\033[39m"

// ANSI escape background color codes
#define BLACK_BG            "\033[40m"
#define RED_BG              "\033[41m"
#define GREEN_BG            "\033[42m"
#define YELLOW_BG           "\033[43m"
#define BLUE_BG             "\033[44m"
#define MAGENTA_BG          "\033[45m"
#define CYAN_BG             "\033[46m"
#define WHITE_BG            "\033[47m"
#define RESET_BG            "\033[49m"

// ANSI escape style color codes
#define COLOR_RESET_ALL     "\033[0m"   // back to defaults
#define COLOR_BOLD 	    "\033[1m"   // implemented as 'bright' on some terminals
#define COLOR_RESET_BOLD    "\033[22m"

// (the following are not widely supported)
#define COLOR_DIM	    "\033[2m"
#define COLOR_UNDERLINE     "\033[3m"
#define COLOR_BLINK	    "\033[4m"
#define COLOR_REVERSE       "\033[7m"

#endif // JSH_COLORS_H_INCLUDED
