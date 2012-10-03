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

#include <SDL.h>

static bool __display_fullscreen = false;
static uint32_t __display_width = 0;
static uint32_t __display_height = 0;
static const int __display_bpp = 32;
static SDL_Surface* __display_surface = NULL;

static void null_keyboard_callback(palKeyboardEvent event, void* userdata)
{
  return;
}

static void null_mouse_button_callback (palMouseButtonEvent event, void* userdata)
{
  return;
}

static void null_mouse_move_callback (palMouseMoveEvent event, void* userdata)
{
  return;
}

static keyboard_event_cb		__keyboard_callback = null_keyboard_callback;
static mouse_button_event_cb	__mouse_button_callback = null_mouse_button_callback;
static mouse_move_event_cb		__mouse_move_callback = null_mouse_move_callback;
static void* __userdata = NULL;

int			palAdiInitialize ()
{  
  int r;
  r = SDL_Init (SDL_INIT_TIMER|SDL_INIT_VIDEO);
  if (r < 0)
  {
    printf("SDL_Init failed: %s\n", SDL_GetError());
    return r;
  }

  SDL_EnableUNICODE (1);
  return 0;
}

void		palAdiShutdown ()
{
  palAdiReleaseMouse();
  palAdiCloseDisplay();
  SDL_Quit();
}

static uint16_t
sdl_mod_to_kb_mod (SDLMod mod)
{
  uint16_t result = 0;

  if (mod & KMOD_CTRL)
  {
    result |= KB_MOD_CTRL;
    if (mod & KMOD_RCTRL)
      result |= KB_MOD_RCTRL;
    else
      result |= KB_MOD_LCTRL;
  }
  if (mod & KMOD_SHIFT)
  {
    result |= KB_MOD_SHIFT;
    if (mod & KMOD_RSHIFT)
      result |= KB_MOD_RSHIFT;
    else
      result |= KB_MOD_LSHIFT;
  }
  if (mod & KMOD_ALT)
  {
    result |= KB_MOD_ALT;
    if (mod & KMOD_RALT)
      result |= KB_MOD_RALT;
    else
      result |= KB_MOD_LALT;
  }

  return result;
}

void		palAdiProcessEvents ()
{
  SDL_Event event;

  while (SDL_PollEvent (&event))
  {
    uint16_t kb_mod;
    SDLMod mod;
    SDLKey sym;
    char ch;

    switch (event.type)
    {
    case SDL_KEYUP:
    case SDL_KEYDOWN:
      mod = event.key.keysym.mod;
      kb_mod = sdl_mod_to_kb_mod (mod);
      sym = event.key.keysym.sym;
      if ((event.key.keysym.unicode & 0xFF80) == 0)
      {
        ch = (char)(event.key.keysym.unicode & 0x7F);
      } else {
        // international character
        ch = 0;
      }
      if (event.type == SDL_KEYDOWN && (sym == 'g' || sym == 'G') && (kb_mod & KB_MOD_CTRL))
      {
        if (palAdiIsMouseGrabbed())
          palAdiReleaseMouse();
        else
          palAdiGrabMouse();
      }
      palKeyboardEvent pkevent;
      pkevent.time = palTimerGetTicks();
      pkevent.down = event.type == SDL_KEYDOWN ? true : false;
      pkevent.mod = (kb_mod_t)kb_mod;
      pkevent.key = (kb_sym_t)sym;
      pkevent.ascii = ch;
      __keyboard_callback (pkevent, __userdata);
      break;
    case SDL_MOUSEBUTTONUP:
    case SDL_MOUSEBUTTONDOWN:
      palMouseButtonEvent pbevent;
      pbevent.time = palTimerGetTicks();
      pbevent.down = event.type == SDL_MOUSEBUTTONDOWN ? true : false;
      pbevent.button_index = event.button.button;
      pbevent.x = event.button.x;
      pbevent.y = event.button.y;
      __mouse_button_callback (pbevent, __userdata);
      break;
    case SDL_MOUSEMOTION:
      palMouseMoveEvent pmevent;
      pmevent.time = palTimerGetTicks();
      pmevent.button_mask = event.motion.state;
      pmevent.x = event.motion.x;
      pmevent.y = event.motion.y;
      pmevent.dx = event.motion.xrel;
      pmevent.dy = event.motion.yrel;
      __mouse_move_callback (pmevent, __userdata);
      break;
    }
  }
}

void		palAdiRegisterKeyboardEventCallback		(keyboard_event_cb event_cb)
{
  __keyboard_callback = event_cb;
}

void		palAdiRegisterMouseButtonCallback	(mouse_button_event_cb event_cb)
{
  __mouse_button_callback = event_cb;
}
void		palAdiRegisterMouseMoveCallback		(mouse_move_event_cb event_cb)
{
  __mouse_move_callback = event_cb;
}
void		palAdiRegisterEventCallbackUserdata		(void* userdata)
{
  __userdata = userdata;
}

void		palAdiGrabMouse								()
{
  SDL_GrabMode mode;

  mode = SDL_WM_GrabInput (SDL_GRAB_ON);

  if (mode == SDL_GRAB_ON)
  {
    SDL_ShowCursor (SDL_DISABLE);
  }
}

void		palAdiReleaseMouse							()
{
  SDL_GrabMode mode;

  mode = SDL_WM_GrabInput (SDL_GRAB_OFF);

  if (mode == SDL_GRAB_OFF)
  {
    SDL_ShowCursor (SDL_ENABLE);
  }
}
bool		palAdiIsMouseGrabbed						()
{
  SDL_GrabMode mode;

  mode = SDL_WM_GrabInput (SDL_GRAB_QUERY);

  if (mode == SDL_GRAB_ON)
    return true;
  else
    return false;
}
void		palAdiWarpMouse								(uint16_t x, uint16_t y)
{
  SDL_WarpMouse (x, y);
}

int			palAdiConfigureDisplay						(int width, int height, bool fullscreen)
{
  __display_width = width;
  __display_height = height;
  __display_fullscreen = fullscreen;

  uint32_t flags = SDL_OPENGL;

  SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);

  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 32);
  SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 0);

  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

  SDL_GL_SetAttribute(SDL_GL_ACCUM_RED_SIZE, 0);
  SDL_GL_SetAttribute(SDL_GL_ACCUM_GREEN_SIZE, 0);
  SDL_GL_SetAttribute(SDL_GL_ACCUM_BLUE_SIZE, 0);
  SDL_GL_SetAttribute(SDL_GL_ACCUM_ALPHA_SIZE, 0);

  if (__display_fullscreen)
  {
    flags |= SDL_FULLSCREEN;
  }


  int r = SDL_VideoModeOK (__display_width, __display_height,__display_bpp, flags);

  if (r == 0)
  {
    printf("Request display mode is not supported on this platform.\n");
    return -1;
  }

  if (r != __display_bpp)
  {
    printf("Request BPP(32) is not supported on this platform.\n");
    return -1;
  }

  return 0;
}

int			palAdiOpenDisplay							()
{
  uint32_t flags = SDL_OPENGL;

  SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);

  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 32);
  SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 0);

  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

  SDL_GL_SetAttribute(SDL_GL_ACCUM_RED_SIZE, 0);
  SDL_GL_SetAttribute(SDL_GL_ACCUM_GREEN_SIZE, 0);
  SDL_GL_SetAttribute(SDL_GL_ACCUM_BLUE_SIZE, 0);
  SDL_GL_SetAttribute(SDL_GL_ACCUM_ALPHA_SIZE, 0);

  if (__display_fullscreen)
  {
    flags |= SDL_FULLSCREEN;
  }

  __display_surface = SDL_SetVideoMode (__display_width, __display_height, __display_bpp, flags);

  if (!__display_surface)
  {
    printf("SDL_SetVideoMode failed: %s\n", SDL_GetError());
    return -1;
  }

  return 0;
}

int     palAdiDisplayWidth() {
  return __display_width;
}

int     palAdiDisplayHeight() {
  return __display_height;
}

int     palAdiDisplayDPI() {
  return 96;
}

int			palAdiCloseDisplay							()
{
  SDL_FreeSurface (__display_surface);
  return 0;
}

bool		palAdiSetFullscreen							(bool fullscreen)
{
  return fullscreen;
}

bool		palAdiIsFullscreen							()
{
  return __display_fullscreen;
}

void		palAdiSwapBuffers							()
{
  SDL_GL_SwapBuffers();
}