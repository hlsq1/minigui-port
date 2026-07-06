/* mgconfig.h — MiniGUI config for the miniOS port.
 * The real config lives in the repo root (mgconfig_minios.h).
 * Both the library build and the application build find this file via -I,
 * and common.h (line 83-101) does #include "mgconfig.h" when __MINIGUI_LIB__
 * is NOT defined. */
#pragma once
#include "mgconfig_minios.h"
