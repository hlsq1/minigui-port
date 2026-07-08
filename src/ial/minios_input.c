/*
 * minios_input.c: MiniGUI COMM IAL external stubs for miniOS.
 *
 * Implements __comminput_* using miniOS /dev/kbd and /dev/mouse devices.
 * Keyboard events are raw scancodes in struct key_event format.
 * Mouse events are relative movements in struct mouse_event format.
 *
 * Compile with -D_MGIAL_COMM
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/time.h>
#include <poll.h>

#include "common.h"
#include "minigui.h"
#include "gal.h"
#include "exstubs.h"

/* miniOS /dev/kbd event structure (from uapi/minios/kbd.h).
 * MUST match kernel's struct kbd_event byte-for-byte:
 *   uint8_t scancode; uint8_t pressed; uint8_t extended;  (3 bytes total)
 * Previously used a 2-byte struct which caused read() misalignment. */
struct key_event {
    uint8_t scancode;
    uint8_t pressed;
    uint8_t extended;
};
#define KEY_EVENT_MAKE  1
#define KEY_EVENT_BREAK 0

/* miniOS /dev/mouse event structure (MUST match kernel's struct mouse_event).
 * Kernel sends: int32_t dx; int32_t dy; int32_t dz; uint32_t buttons; (16 bytes).
 * Previously used a 3-byte struct which caused read() misalignment. */
struct mouse_event {
    int32_t dx;
    int32_t dy;
    int32_t dz;
    uint32_t buttons;
};
#define MOUSE_BUTTON_LEFT   0x01
#define MOUSE_BUTTON_RIGHT  0x02
#define MOUSE_BUTTON_MIDDLE 0x04

static int kbd_fd = -1;
static int mouse_fd = -1;

/* Absolute mouse position tracking (accumulated from relative events) */
static short mouse_x = 0, mouse_y = 0;

/* Cached events consumed by wait_for_input polling, consumed by ts_getdata/kb_getdata */
static struct mouse_event cached_mouse_ev;
static int cached_mouse_valid = 0;
static struct key_event cached_kbd_ev;
static int cached_kbd_valid = 0;

int __comminput_init(void)
{
    kbd_fd = open("/dev/kbd", O_RDONLY | O_NONBLOCK);
    mouse_fd = open("/dev/mouse", O_RDONLY | O_NONBLOCK);

    {
        char buf[64];
        int n = sprintf(buf, "IAL: init kbd_fd=%d mouse_fd=%d\n", kbd_fd, mouse_fd);
        write(1, buf, n);
    }

    if (kbd_fd < 0 || mouse_fd < 0) {
        if (kbd_fd >= 0) close(kbd_fd);
        if (mouse_fd >= 0) close(mouse_fd);
        kbd_fd = -1;
        mouse_fd = -1;
        return -1;
    }

    /* Drain scancodes/packets buffered while the console (shell) had focus.
     * /dev/kbd and /dev/mouse are global kernel ring buffers shared with the
     * console, so every key typed at the shell accumulates there. Without
     * this, the whole backlog (e.g. the command line + its Enter) floods the
     * first GUI window on startup and can dismiss it before it is seen. */
    {
        struct key_event ke;
        struct mouse_event me;
        while (read(kbd_fd, &ke, sizeof(ke)) == (int)sizeof(ke)) { }
        while (read(mouse_fd, &me, sizeof(me)) == (int)sizeof(me)) { }
    }

    return 0;
}

void __comminput_deinit(void)
{
    if (kbd_fd >= 0) {
        close(kbd_fd);
        kbd_fd = -1;
    }
    if (mouse_fd >= 0) {
        close(mouse_fd);
        mouse_fd = -1;
    }
}

int __comminput_ts_getdata(short *x, short *y, short *button)
{
    if (!cached_mouse_valid)
        return -1;

    cached_mouse_valid = 0;

    /* Accumulate relative movement into absolute position */
    mouse_x += cached_mouse_ev.dx;
    mouse_y += cached_mouse_ev.dy;

    /* Clamp to reasonable range */
    if (mouse_x < 0) mouse_x = 0;
    if (mouse_y < 0) mouse_y = 0;
    if (mouse_x > 1023) mouse_x = 1023;
    if (mouse_y > 767) mouse_y = 767;

    *x = mouse_x;
    *y = mouse_y;

    /* Map miniOS button bits to COMM button bits */
    *button = 0;
    if (cached_mouse_ev.buttons & MOUSE_BUTTON_LEFT)
        *button |= COMM_MOUSELBUTTON;
    if (cached_mouse_ev.buttons & MOUSE_BUTTON_RIGHT)
        *button |= COMM_MOUSERBUTTON;

    return 0;
}

int __comminput_kb_getdata(short *key, short *status)
{
    if (!cached_kbd_valid)
        return -1;

    cached_kbd_valid = 0;

    short sc = cached_kbd_ev.scancode;

    /* [ADDED] E0-prefixed navigation keys arrive with the keypad base
     * scancode (Up = 0x48 = 72, etc.) plus the extended flag set.  MiniGUI
     * uses Linux-style keycodes where the dedicated arrow/nav cluster is the
     * "cursor block" range (102..111), distinct from the keypad range
     * (71..83).  Controls such as LISTBOX only handle SCANCODE_CURSORBLOCK*,
     * so without this remap the arrow keys are seen as keypad keys and
     * ignored.  Non-extended keypad keys keep their keypad scancodes. */
    if (cached_kbd_ev.extended) {
        switch (sc) {
        case 72: sc = SCANCODE_CURSORBLOCKUP;    break; /* E0 48 -> 103 */
        case 80: sc = SCANCODE_CURSORBLOCKDOWN;  break; /* E0 50 -> 108 */
        case 75: sc = SCANCODE_CURSORBLOCKLEFT;  break; /* E0 4B -> 105 */
        case 77: sc = SCANCODE_CURSORBLOCKRIGHT; break; /* E0 4D -> 106 */
        case 71: sc = SCANCODE_HOME;             break; /* E0 47 -> 102 */
        case 79: sc = SCANCODE_END;              break; /* E0 4F -> 107 */
        case 73: sc = SCANCODE_PAGEUP;           break; /* E0 49 -> 104 */
        case 81: sc = SCANCODE_PAGEDOWN;         break; /* E0 51 -> 109 */
        case 82: sc = SCANCODE_INSERT;           break; /* E0 52 -> 110 */
        case 83: sc = SCANCODE_REMOVE;           break; /* E0 53 -> 111 */
        default: break;
        }
    }

    *key = sc;
    *status = (cached_kbd_ev.pressed == KEY_EVENT_MAKE) ? 1 : 0;

    return 0;
}

int __comminput_wait_for_input(struct timeval *timeout)
{
    int timeout_ms;

    if (timeout == NULL || (timeout->tv_sec == 0 && timeout->tv_usec == 0)) {
        /* No hard timeout: wait a short slice. MiniGUI relies on wait_for_input
         * returning periodically even with no input so the kernel can check
         * its timers (caret blink etc.). */
        timeout_ms = 50;
    } else {
        timeout_ms = timeout->tv_sec * 1000 + timeout->tv_usec / 1000;
        if (timeout_ms < 10)
            timeout_ms = 10;  /* minimum wait slice */
    }

    /* Wait on the fds that still need a fresh event (skip any already cached,
     * matching the previous read-loop). One poll() replaces the old
     * read+sleep spin: the kernel sleeps us until an fd is ready or the
     * timeout elapses, then we read only the fds it flagged POLLIN. */
    struct pollfd pfds[2];
    int mouse_slot = -1, kbd_slot = -1;
    int nfds = 0;
    if (mouse_fd >= 0 && !cached_mouse_valid) {
        pfds[nfds].fd = mouse_fd; pfds[nfds].events = POLLIN; pfds[nfds].revents = 0;
        mouse_slot = nfds++;
    }
    if (kbd_fd >= 0 && !cached_kbd_valid) {
        pfds[nfds].fd = kbd_fd; pfds[nfds].events = POLLIN; pfds[nfds].revents = 0;
        kbd_slot = nfds++;
    }

    if (poll(pfds, nfds, timeout_ms) <= 0)
        return 0;  /* timeout (0) or error (-1): no new input this slice */

    int got_data = 0;
    if (mouse_slot >= 0 && (pfds[mouse_slot].revents & POLLIN)) {
        int n = read(mouse_fd, &cached_mouse_ev, sizeof(cached_mouse_ev));
        if (n == sizeof(cached_mouse_ev)) {
            cached_mouse_valid = 1;
            got_data |= COMM_MOUSEINPUT;
        }
    }

    if (kbd_slot >= 0 && (pfds[kbd_slot].revents & POLLIN)) {
        int n = read(kbd_fd, &cached_kbd_ev, sizeof(cached_kbd_ev));
        if (n == sizeof(cached_kbd_ev)) {
            cached_kbd_valid = 1;
            got_data |= COMM_KBINPUT;
        }
    }

    return got_data;
}
