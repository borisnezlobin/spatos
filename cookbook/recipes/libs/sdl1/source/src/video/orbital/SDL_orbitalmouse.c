/*
    SDL - Simple DirectMedia Layer
    Copyright (C) 1997-2012 Sam Lantinga

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

    Sam Lantinga
    slouken@libsdl.org
*/
#include "SDL_config.h"

#include "SDL_mouse.h"
#include "../../events/SDL_events_c.h"

#include "SDL_orbitalmouse_c.h"


/* The implementation dependent data for the window manager cursor */
struct WMcursor {
    int unused;
};

WMcursor * ORBITAL_CreateWMCursor (_THIS,
        Uint8 * data, Uint8 * mask, int w, int h, int hot_x, int hot_y)
{
    WMcursor * cursor ;

    cursor = (WMcursor *) SDL_malloc (sizeof (WMcursor)) ;
    if (cursor == NULL) {
        SDL_OutOfMemory () ;
        return NULL ;
    }

    return cursor ;
}

void ORBITAL_FreeWMCursor (_THIS, WMcursor * cursor)
{
    SDL_free (cursor) ;
}

int ORBITAL_ShowWMCursor (_THIS, WMcursor * cursor)
{
    bool mouse_cursor = cursor != NULL;
    // printf("ORBITAL_ShowWMCursor(%p) = %d\n", cursor, mouse_cursor);
    if (mouse_cursor != this->hidden->mouse_cursor) {
        this->hidden->mouse_cursor = mouse_cursor;
        if (this->hidden->window) {
            orb_window_set_mouse_cursor(this->hidden->window, mouse_cursor);
        }
    }
}
