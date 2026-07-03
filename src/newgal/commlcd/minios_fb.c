/*
 * minios_fb.c: MiniGUI CommLCD external stubs for miniOS.
 *
 * Implements __commlcd_drv_* using miniOS /dev/fb0 framebuffer device.
 * Uses mmap to map the linear framebuffer into process address space.
 *
 * Compile with -D_MGGAL_COMMLCD
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include "common.h"
#include "minigui.h"
#include "gal.h"
#include "commlcd.h"
#include "exstubs.h"

/* miniOS does not have <sys/mman.h>; use the io.h wrapper */
#include <io.h>

/* miniOS framebuffer ioctl structures, matching include/minios/fb.h */
struct fb_fix_screeninfo {
    char     id[16];
    uint32_t smem_start;
    uint32_t smem_len;
    uint32_t type;
    uint32_t visual;
    uint16_t line_length;
};

struct fb_var_screeninfo {
    uint32_t xres;
    uint32_t yres;
    uint32_t xres_virtual;
    uint32_t yres_virtual;
    uint32_t bits_per_pixel;
    struct {
        uint32_t offset;
        uint32_t length;
        uint32_t msb_right;
    } red, green, blue;
};

#define FBIOGET_FSCREENINFO  0x4600
#define FBIOGET_VSCREENINFO  0x4601

static int fb_fd = -1;
static uint8_t *fb_mem = NULL;
static struct fb_fix_screeninfo fb_fix;
static struct fb_var_screeninfo fb_var;

int __commlcd_drv_init(void)
{
    fb_fd = open("/dev/fb0", O_RDWR);
    if (fb_fd < 0)
        return -1;

    if (ioctl(fb_fd, FBIOGET_FSCREENINFO, &fb_fix) < 0)
        return -1;
    if (ioctl(fb_fd, FBIOGET_VSCREENINFO, &fb_var) < 0)
        return -1;

    fb_mem = mmap(NULL, fb_fix.smem_len,
                  PROT_READ | PROT_WRITE, MAP_SHARED, fb_fd, 0);
    if (fb_mem == (uint8_t *)-1)
        return -1;

    return 0;
}

int __commlcd_drv_getinfo(struct commlcd_info *li, int width, int height, int depth)
{
    (void)width;
    (void)height;
    (void)depth;

    /* miniOS mmap may return 0 as valid address - check for unitialized only */
    if (fb_fd < 0)
        return -1;

    li->height = fb_var.yres;
    li->width  = fb_var.xres;
    li->pitch  = fb_fix.line_length;
    li->bpp    = fb_var.bits_per_pixel;
    li->fb     = fb_mem;

    /* Determine pixel type from bpp and RGB offsets */
    if (fb_var.bits_per_pixel == 32) {
        if (fb_var.red.offset == 16 && fb_var.green.offset == 8)
            li->type = COMMLCD_TRUE_ARGB8888;  /* little-endian: B,G,R,A in memory */
        else
            li->type = COMMLCD_TRUE_ARGB8888;
    } else if (fb_var.bits_per_pixel == 24) {
        li->type = COMMLCD_TRUE_RGB888;
    } else if (fb_var.bits_per_pixel == 16) {
        li->type = COMMLCD_TRUE_RGB565;
    } else {
        li->type = COMMLCD_TRUE_ARGB8888; /* fallback */
    }

    li->update_method = COMMLCD_UPDATE_SYNC;

    return 0;
}

int __commlcd_drv_release(void)
{
    if (fb_fd >= 0) {
        fb_mem = NULL;
    }
    if (fb_fd >= 0) {
        close(fb_fd);
        fb_fd = -1;
    }
    return 0;
}

int __commlcd_drv_setclut(int firstcolor, int ncolors, GAL_Color *colors)
{
    (void)firstcolor;
    (void)ncolors;
    (void)colors;
    /* Not needed for truecolor */
    return 0;
}

int __commlcd_drv_update(const RECT *rc_dirty)
{
    /* In synchronous mode with direct framebuffer, no explicit flush needed.
     * MiniGUI draws directly to fb_mem which is the hardware framebuffer. */
    (void)rc_dirty;
    return 0;
}

/* Define the ops table that commlcd.c references */
struct commlcd_ops __mg_commlcd_ops = {
    .init    = __commlcd_drv_init,
    .getinfo = __commlcd_drv_getinfo,
    .release = __commlcd_drv_release,
    .setclut = __commlcd_drv_setclut,
    .update  = __commlcd_drv_update,
};
