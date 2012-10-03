/*
	Copyright (c) 2011 John McCutchan <john@johnmccutchan.com>

	This software is provided 'as-is', without any express or implied
	warranty. In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	claim that you wrote the original software. If you use this software
	in a product, an acknowledgment in the product documentation would be
	appreciated but is not required.

	2. Altered source versions must be plainly marked as such, and must not be
	misrepresented as being the original software.

	3. This notice may not be removed or altered from any source
	distribution.
*/

#pragma once

#include "libpal/pal_platform.h"
#include "libpal/pal_types.h"
#include "libpal/pal_adi_keyboard_symbols.h"
#include "libpal/pal_adi_mouse_symbols.h"
#include "libpal/pal_timer.h"

struct palKeyboardEvent {
	palTimerTick time;
	bool down;
	kb_mod_t mod;
	kb_sym_t key;
	char ascii;
};

struct palMouseButtonEvent {
	palTimerTick time;
	bool down;
	uint8_t button_index;
	uint16_t x;
	uint16_t y;
};

struct palMouseMoveEvent {
	palTimerTick time;
	uint8_t	button_mask;
	uint16_t x;
	uint16_t y;
	int16_t	dx;
	int16_t	dy;
};

typedef void (*keyboard_event_cb)(palKeyboardEvent event, void* userdata);
typedef void (*mouse_button_event_cb)(palMouseButtonEvent event, void* userdata);
typedef void (*mouse_move_event_cb)(palMouseMoveEvent event, void* userdata);

int			palAdiInitialize(void* a, void* b, int c);
void		palAdiShutdown();

void		palAdiProcessEvents();
void		palAdiRegisterKeyboardEventCallback(keyboard_event_cb event_cb);
void		palAdiRegisterMouseButtonCallback(mouse_button_event_cb event_cb);
void		palAdiRegisterMouseMoveCallback(mouse_move_event_cb event_cb);
void		palAdiRegisterEventCallbackUserdata(void* userdata);

void		palAdiGrabMouse();
void		palAdiReleaseMouse();
bool		palAdiIsMouseGrabbed();
void		palAdiWarpMouse(uint16_t x, uint16_t y);

int			palAdiConfigureDisplay(int width, int height, bool fullscreen = false);
int			palAdiOpenDisplay();
bool    palAdiIsDisplayClosed();
int     palAdiDisplayWidth();
int     palAdiDisplayHeight();
int     palAdiDisplayDPI();
int			palAdiCloseDisplay();
bool		palAdiSetFullscreen(bool fullscreen);
bool		palAdiIsFullscreen();
void		palAdiSwapBuffers();

void*   palAdiGetDisplayHandle();
void*   palAdiGetSwapChainHandle();
void*   palAdiGetDeviceHandle();
