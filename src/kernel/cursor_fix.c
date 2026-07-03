/* Override kernel_RefreshCursor to fix IAL_GetMouseXY crash.
 * This replaces the version in cursor-sa.o from libminigui.a.
 */
#include "common.h"

#ifdef _MGRM_STANDALONE

#include "minigui.h"
#include "gdi.h"
#include "window.h"
#include "cliprect.h"
#include "internals.h"
#include "gal.h"
#include "cursor.h"
#include "ial.h"

extern GAL_Surface* __gal_screen;
extern INPUT* __mg_cur_input;

BOOL kernel_RefreshCursor (int* x, int* y, int* button)
{
    BOOL moved = FALSE;

    /* Direct function pointer call instead of IAL_GetMouseXY inline */
    if (__mg_cur_input && __mg_cur_input->get_mouse_xy) {
        (*__mg_cur_input->get_mouse_xy) (x, y);
    }
    if (*x < 0) *x = 0;
    if (*y < 0) *y = 0;
    if (*x > (__gal_screen->w - 1)) *x = (__gal_screen->w - 1);
    if (*y > (__gal_screen->h - 1)) *y = (__gal_screen->h - 1);
    int curx = *x;
    int cury = *y;

    if (button) {
        *button = IAL_GetMouseButton ();
    }

    {
        extern int oldx, oldy;
        if (oldx != curx || oldy != cury) {
#ifdef _MGHAVE_CURSOR
            extern int nShowCount;
            extern void* pCurCsr;
            if (nShowCount >= 0 && pCurCsr) {
                extern void hidecursor(void);
                extern void showcursor(void);
                hidecursor ();
                showcursor ();
                GAL_SyncUpdate (__gal_screen);
            }
#endif
            oldx = curx;
            oldy = cury;
            moved = TRUE;
        }
    }

    return moved;
}

#endif /* _MGRM_STANDALONE */
