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


// DX11, Windows
#include <windows.h>
#include <windowsx.h>
#include <dxgi.h>
#pragma comment(lib, "dxgi.lib")
#include <d3d11.h>
#pragma comment(lib, "d3d11.lib")

#include "libpal/pal_memory.h"

static bool __display_fullscreen = false;
static uint32_t __display_width = 0;
static uint32_t __display_height = 0;
static const int __display_bpp = 32;
static bool __display_closed = false;

static HWND __window;
static IDXGIAdapter1* __adapter;
static IDXGISwapChain* __swap_chain;
static ID3D11Device* __device;
static ID3D11DeviceContext* _device_context;

static void null_keyboard_callback(palKeyboardEvent event, void* userdata) {
  return;
}

static void null_mouse_button_callback(palMouseButtonEvent event, void* userdata) {
  return;
}

static void null_mouse_move_callback(palMouseMoveEvent event, void* userdata) {
  return;
}

static keyboard_event_cb		__keyboard_callback = null_keyboard_callback;
static mouse_button_event_cb	__mouse_button_callback = null_mouse_button_callback;
static mouse_move_event_cb		__mouse_move_callback = null_mouse_move_callback;
static void* __userdata = NULL;

// used to initialize windows
static HINSTANCE hInstance;
static HINSTANCE hPrevInstance;
static int nCmdShow;

int palAdiInitialize(void* a, void* b, int c) {
  hInstance = reinterpret_cast<HINSTANCE>(a);
  hPrevInstance = reinterpret_cast<HINSTANCE>(b);
  nCmdShow = c;
  __display_closed = true;
  return 0;
}

void palAdiShutdown() {
  palAdiReleaseMouse();
  palAdiCloseDisplay();
}

static kb_mod_t UpdateRunningMod(kb_mod_t current, bool down, WPARAM w, LPARAM l) {
  kb_mod_t key = KB_MOD_NONE;
  switch (w) {
    case KB_SYM_SHIFT:
    case KB_SYM_LSHIFT:
      key = KB_MOD_LSHIFT;
    break;
    case KB_SYM_RSHIFT:
      key = KB_MOD_RSHIFT;
    break;
    case KB_SYM_RALT:
      key = KB_MOD_RALT;
    break;
    case KB_SYM_ALT:
    case KB_SYM_LALT:
      key = KB_MOD_LALT;
    break;
    case KB_SYM_CTRL:
    case KB_SYM_LCTRL:
      key = KB_MOD_LCTRL;
    break;
    case KB_SYM_RCTRL:
      key = KB_MOD_RCTRL;
    break;
    default:
    break;
  }
  if (key == 0)
    return current;

  if (down) {
    return (kb_mod_t)(current | key);
  } else {
    return (kb_mod_t)(current & (~key));
  }
}

static uint8_t MakeButtonMask(WPARAM w) {
  uint8_t mask = 0;
  if (w & MK_LBUTTON) {
    mask |= MB_LEFT_MASK;
  }
  if (w & MK_MBUTTON) {
    mask |= MB_MIDDLE_MASK;
  }
  if (w & MK_RBUTTON) {
    mask |= MB_RIGHT_MASK;
  }
  return mask;
}

void palAdiProcessEvents() {
  static kb_mod_t running_mod = KB_MOD_NONE;
  static short last_x = 0;
  static short last_y = 0;
  MSG msg;
  MSG checkmessage;
  while(1) {
    if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
      switch (msg.message) {
      case WM_KEYDOWN:
        {
        running_mod = UpdateRunningMod(running_mod, true, msg.wParam, msg.lParam);
        palKeyboardEvent pkevent;
        pkevent.time = palTimerGetTicks();
        pkevent.down = true;
        pkevent.mod = (kb_mod_t)running_mod;
        pkevent.key = (kb_sym_t)(msg.wParam);
        pkevent.ascii = 0;
        TranslateMessage(&msg);
        if (PeekMessage(&checkmessage, NULL, 0, 0, PM_NOREMOVE)) {
          if (checkmessage.message == WM_CHAR) {
            // remove the message
            PeekMessage(&checkmessage, NULL, 0, 0, PM_REMOVE);
            pkevent.ascii = (char)checkmessage.wParam;
          }
        }
        if ((running_mod & KB_MOD_CTRL) != 0 && pkevent.key == KB_SYM_g) {
          if (palAdiIsMouseGrabbed()) {
            palAdiReleaseMouse();
            palPrintf("Released mouse\n");
          } else {
            palAdiGrabMouse();
            palPrintf("Grabbed mouse\n");
          }
        }
        //palPrintf("mod: %x\n", running_mod);
        __keyboard_callback(pkevent, __userdata);
        } break;
      case WM_KEYUP:
        {
        running_mod = UpdateRunningMod(running_mod, false, msg.wParam, msg.lParam);
        palKeyboardEvent pkevent;
        pkevent.time = palTimerGetTicks();
        pkevent.down = false;
        pkevent.mod = (kb_mod_t)running_mod;
        pkevent.key = (kb_sym_t)(msg.wParam);
        pkevent.ascii = 0;
        __keyboard_callback(pkevent, __userdata);
        } break;
      case WM_MOUSEMOVE:
        {
          palMouseMoveEvent pmevent;
          pmevent.time = palTimerGetTicks();
          pmevent.button_mask = MakeButtonMask(msg.wParam);
          pmevent.x = GET_X_LPARAM(msg.lParam);
          pmevent.y = GET_Y_LPARAM(msg.lParam);
          pmevent.dx = pmevent.x - last_x;
          pmevent.dy = pmevent.y - last_y;
          last_x = pmevent.x;
          last_y = pmevent.y;
          __mouse_move_callback (pmevent, __userdata);
        } break;
      case WM_LBUTTONDOWN:
        {
          palMouseButtonEvent pbevent;
          pbevent.time = palTimerGetTicks();
          pbevent.down = true;
          pbevent.button_index = MB_LEFT_INDEX;
          pbevent.x = GET_X_LPARAM(msg.lParam);
          pbevent.y = GET_Y_LPARAM(msg.lParam);
          __mouse_button_callback (pbevent, __userdata);
        } break;
      case WM_LBUTTONUP:
        {
          palMouseButtonEvent pbevent;
          pbevent.time = palTimerGetTicks();
          pbevent.down = false;
          pbevent.button_index = MB_LEFT_INDEX;
          pbevent.x = GET_X_LPARAM(msg.lParam);
          pbevent.y = GET_Y_LPARAM(msg.lParam);
          __mouse_button_callback (pbevent, __userdata);
        } break;
      case WM_RBUTTONDOWN:
        {
          palMouseButtonEvent pbevent;
          pbevent.time = palTimerGetTicks();
          pbevent.down = true;
          pbevent.button_index = MB_RIGHT_INDEX;
          pbevent.x = GET_X_LPARAM(msg.lParam);
          pbevent.y = GET_Y_LPARAM(msg.lParam);
          __mouse_button_callback (pbevent, __userdata);
        } break;
      case WM_RBUTTONUP:
        {
          palMouseButtonEvent pbevent;
          pbevent.time = palTimerGetTicks();
          pbevent.down = false;
          pbevent.button_index = MB_RIGHT_INDEX;
          pbevent.x = GET_X_LPARAM(msg.lParam);
          pbevent.y = GET_Y_LPARAM(msg.lParam);
          __mouse_button_callback (pbevent, __userdata);
        } break;
      case WM_MBUTTONDOWN:
        {
          palMouseButtonEvent pbevent;
          pbevent.time = palTimerGetTicks();
          pbevent.down = true;
          pbevent.button_index = MB_MIDDLE_INDEX;
          pbevent.x = GET_X_LPARAM(msg.lParam);
          pbevent.y = GET_Y_LPARAM(msg.lParam);
          __mouse_button_callback (pbevent, __userdata);
        } break;
      case WM_MBUTTONUP:
        {
          palMouseButtonEvent pbevent;
          pbevent.time = palTimerGetTicks();
          pbevent.down = false;
          pbevent.button_index = MB_MIDDLE_INDEX;
          pbevent.x = GET_X_LPARAM(msg.lParam);
          pbevent.y = GET_Y_LPARAM(msg.lParam);
          __mouse_button_callback (pbevent, __userdata);
        } break;
      }
      DispatchMessage(&msg);
    } else {
      // no more messages
      break;
    }
  }
}

void palAdiRegisterKeyboardEventCallback(keyboard_event_cb event_cb) {
  __keyboard_callback = event_cb;
}

void palAdiRegisterMouseButtonCallback(mouse_button_event_cb event_cb) {
  __mouse_button_callback = event_cb;
}

void palAdiRegisterMouseMoveCallback(mouse_move_event_cb event_cb) {
  __mouse_move_callback = event_cb;
}

void palAdiRegisterEventCallbackUserdata(void* userdata) {
  __userdata = userdata;
}

void palAdiGrabMouse() {
  HWND old_window;
  old_window = SetCapture(__window);
}

void palAdiReleaseMouse() {
  bool released = (ReleaseCapture() != 0);
}

bool palAdiIsMouseGrabbed() {
  HWND grabbed_window = GetCapture();
  return grabbed_window == __window;
}

void palAdiWarpMouse(uint16_t x, uint16_t y) {
  HWND hwnd = __window;
  POINT pt;
  pt.x = x;
  pt.y = y;
  ClientToScreen(hwnd, &pt);
  SetCursorPos(pt.x, pt.y);
}

static LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
  switch(message) {
    case WM_DESTROY: {
        PostQuitMessage(0);
        return 0;
    } break;
  }
  return DefWindowProc (hWnd, message, wParam, lParam);
}

int palAdiConfigureDisplay(int width, int height, bool fullscreen) {
  __display_width = width;
  __display_height = height;
  __display_fullscreen = fullscreen;

  // this struct holds information for the window class
  WNDCLASSEX wc;

  // clear out the window class for use
  ZeroMemory(&wc, sizeof(WNDCLASSEX));

  // fill in the struct with the needed information
  wc.cbSize = sizeof(WNDCLASSEX);
  wc.style = CS_HREDRAW | CS_VREDRAW;
  wc.lpfnWndProc = WindowProc;
  wc.hInstance = hInstance;
  wc.hCursor = LoadCursor(NULL, IDC_ARROW);
  //wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
  wc.lpszClassName = "palWindowClass";
  // register the window class
  RegisterClassEx(&wc);

  RECT wr = {0, 0, width, height};
  AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);

  __window = CreateWindowEx(NULL,
    "palWindowClass",
    "Domingo Engine",
    WS_OVERLAPPEDWINDOW,
    300,
    300,
    wr.right - wr.left,
    wr.bottom - wr.top,
    NULL,
    NULL,
    hInstance,
    NULL);

  if (__window == NULL) {
    palPrintf("Could not create window 0x%08x\n", GetLastError());
    return -1;
  }
  IDXGIFactory1 * pFactory;
  HRESULT hr = CreateDXGIFactory1(__uuidof(IDXGIFactory1), (void**)(&pFactory));
  if (hr != S_OK) {
    palPrintf("Could not create DXGI Factory1 0x%08x\n", hr);
    return hr;
  }

  hr = pFactory->EnumAdapters1(0, (IDXGIAdapter1**)(&__adapter));
  if (hr != S_OK) {
    palPrintf("Could not enumerate Adapter 0 0x%08x\n", hr);
    return hr;
  }

  DXGI_ADAPTER_DESC1 desc1;
  hr = __adapter->GetDesc1(&desc1);
  if (hr != S_OK) {
    palPrintf("Could not get description of Adapter 0 0x%08x\n", hr);
    return hr;
  }

  // wchar_t for fucks sake.
  char description[128];
  for (int i = 0; i < 128; i++) {
    if ((desc1.Description[i] & 0xff) == 0) {
      description[i] = '\0';
      break;
    }
    description[i] = (desc1.Description[i] & 0xff);
  }
  

  const D3D_FEATURE_LEVEL requested_level = D3D_FEATURE_LEVEL_11_0;
  D3D_FEATURE_LEVEL actual_level;

  DXGI_SWAP_CHAIN_DESC scd;
  palMemoryZeroBytes(&scd, sizeof(scd));
  scd.BufferCount = 2;
  scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
  scd.OutputWindow = __window;
  scd.BufferDesc.Height = __display_height;
  scd.BufferDesc.Width = __display_width;
  scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
  scd.BufferDesc.Scaling = DXGI_MODE_SCALING_CENTERED;
  scd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
  // any refresh rate will do
  scd.BufferDesc.RefreshRate.Numerator = 0;
  scd.BufferDesc.RefreshRate.Denominator = 1;
  if (fullscreen) {
    scd.Windowed = FALSE;
  } else {
    scd.Windowed = TRUE;
  }
  scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
  // automatic switch between fullscreen and windowed with alt+enter
  scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
  // multisampling disabled
  scd.SampleDesc.Count = 1;
  scd.SampleDesc.Quality = 0;


  hr = D3D11CreateDevice(__adapter,
                         D3D_DRIVER_TYPE_UNKNOWN,
                         NULL,
                         0,
                         NULL,
                         0,
                         D3D11_SDK_VERSION,
                         &__device,
                         &actual_level,
                         &_device_context);
  if (hr != S_OK) {
    palPrintf("Could not create device 0x%08x\n", hr);
    return -1;
  }
  if (requested_level != actual_level) {
    palPrintf("Device %s does not support Direct3D 11 feature level. Exiting...\n", description);
    return -2;
  }
  hr = pFactory->CreateSwapChain(__device, &scd, &__swap_chain);
  if (hr != S_OK) {
    palPrintf("Could not create swap chain 0x%08x\n", hr);
    return -3;
  }
  palPrintf("D3D11: Using adapter - %s\n", description);
  palPrintf("D3D11: Device supports Direct3D 11 feature level.\n");
  palPrintf("Device @ %p\n", __device);
  palPrintf("DeviceContext @ %p\n", _device_context);
  palPrintf("SwapChain @ %p\n", __swap_chain);
  palPrintf("Display configured for (%d x %d)\n", __display_width, __display_height);

  return 0;
}

void* palAdiGetDisplayHandle() {
  return __window;
}

void*   palAdiGetSwapChainHandle() {
  return __swap_chain;
}

void*   palAdiGetDeviceHandle() {
  return __device;
}

int palAdiOpenDisplay() {
  __display_closed = false;
  ShowWindow(__window, nCmdShow);
  return 0;
}

bool palAdiIsDisplayClosed() {
  return __display_closed;
}

int palAdiDisplayWidth() {
  return __display_width;
}

int palAdiDisplayHeight() {
  return __display_height;
}

int palAdiDisplayDPI() {
  return 96;
}

int palAdiCloseDisplay() {
  return 0;
}

bool palAdiSetFullscreen(bool fullscreen) {
  return fullscreen;
}

bool palAdiIsFullscreen() {
  return __display_fullscreen;
}

void palAdiSwapBuffers() {
  __swap_chain->Present(0, 0);
}
