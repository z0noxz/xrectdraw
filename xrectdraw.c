/*
 * MIT License
 *
 * © 2019 Chris Noxz <chris@noxz.tech>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include "config.def.h"

struct Border {
    int top;
    int right;
    int bottom;
    int left;
};

static int screen, gfocus;
static Display *dpy;
static Window root, win[4];
static XColor color[2];
static struct Border border;

void
die(char *format, ...)
{
    va_list args;

    va_start(args, format);
    vprintf(format, args);
    va_end(args);

    exit(1);
}

void
draw(int x, int y, int width, int height)
{
    int i, ix, iy, iw, ih;
    XWindowAttributes wa;
    XSetWindowAttributes swa;
    XClassHint ch = {"xrectdraw", "xrectdraw"};

    swa.border_pixel = 0;
    swa.background_pixel = color[0].pixel;
    swa.override_redirect = 1;
    swa.event_mask = ExposureMask
        | KeyPressMask
        | ButtonPressMask
        | FocusChangeMask;

    XGetWindowAttributes(dpy, root, &wa);

    for (i = 0; i < 4; i++) {
        switch (i) {
        case 0: // setup for left border
            ix = x - border.left;
            iy = y - border.top;
            iw = border.left;
            ih = height + border.bottom;
            break;
        case 1: // setup for right border
            ix += (width + border.left);
            iw = border.right;
            break;
        case 2: // setup for top border
            ix -= (width + border.left);
            iw = (width + border.left + border.right);
            ih = border.top;
            break;
        case 3: // setup for bottom border
            ih = border.bottom;
            iy += (height + border.top);
            break;
        }

        win[i] = XCreateWindow(dpy, root, ix, iy, iw, ih, 0,
            CopyFromParent, CopyFromParent, CopyFromParent,
            CWOverrideRedirect | CWBackPixel | CWEventMask, &swa);

        XSetClassHint(dpy, win[i], &ch);
        XMapRaised(dpy, win[i]);
        XSetInputFocus(dpy, win[i], RevertToParent, CurrentTime);
    }
}

int
hex(const char c)
{
    if (c >= 0x30 && c <= 0x39)
        return c - 0x30;
    else if (c >= 0x41 && c <= 0x46)
        return c - 0x41 + 10;
    else if (c >= 0x61 && c <= 0x66)
        return c - 0x61 + 10;
    else
        puts("error: cannot set color");
    return 0;
}

void
setcolor(const char *clr)
{
    int i, r;
    const char *ptr;

    color[0].flags = DoRed | DoGreen | DoBlue;
    color[1].flags = DoRed | DoGreen | DoBlue;

    for (i = 0; i < 3; i++) {
        ptr = clr + (1 + i * 2);
        r = (hex(ptr[0]) * 16 + hex(ptr[1])) * 256;

        switch (i) {
        case 0:
            color[0].red = r;
            color[1].red = r * dim_percentage;
            break;
        case 1:
            color[0].green = r;
            color[1].green = r * dim_percentage;
            break;
        case 2:
            color[0].blue = r;
            color[1].blue = r * dim_percentage;
            break;
        }
    }

    XAllocColor(dpy, DefaultColormap(dpy, screen), &color[0]);
    XAllocColor(dpy, DefaultColormap(dpy, screen), &color[1]);
}

void
toggle()
{
    if (!gfocus)
        XSetInputFocus(dpy, win[0], RevertToParent, CurrentTime);
    else
        XSetInputFocus(dpy, None, RevertToParent, CurrentTime);
}

void
highlight(int focus)
{
    unsigned long pixel = color[!(gfocus = focus)].pixel;

    for (int i = 0; i < 4; i++) {
        XSetWindowBackground(dpy, win[i], pixel);
        XClearArea(dpy, win[i], 0, 0, 0, 0, True);
    }
}

void
setborders(const char *str)
{
    if (sscanf(str, "%d:%d:%d:%d",
        &border.top,
        &border.right,
        &border.bottom,
        &border.left) != 4)
        die("error: borders should be entered in the format 't:r:b:l'");
}

int
main(int argc, const char *argv[])
{
    int x, y, width, height;

    border.top = BORDER_TOP;
    border.right = BORDER_RIGHT;
    border.bottom = BORDER_BOTTOM;
    border.left = BORDER_LEFT;

    if (argc == 7 && argc--)
        setborders(argv[6]);

    if (argc != 6
        || strcmp(argv[1], "-h") == 0
        || strcmp(argv[1], "--help") == 0)
        die(
            "usage: %s x y width height #RRGGBB [t:r:b:l]\n"
            "  x        x coordinage on screen\n"
            "  y        y coordinage on screen\n"
            "  width    width of the rectangle\n"
            "  height   height of the rectangle (downwards)\n"
            "  #RRGGBB  hex border colors of the rectangle\n"
            "  t:r:b:l  (optional) border widths: top, right, bottom & left\n"
            "           e.g. 2:2:5:2. Default is 1:1:1:1\n\n"
            "Use ESC key to kill the program when focused. Clicking on the"
            "borders toggles the focus."
        , argv[0]);

    if (!(dpy = XOpenDisplay(NULL)))
        die("error: cannot open display");

    screen = DefaultScreen(dpy);
    root = RootWindow(dpy, screen);

    x = atoi(argv[1]);
    y = atoi(argv[2]);
    width = atoi(argv[3]);
    height = atoi(argv[4]);
    setcolor(argv[5]);

    draw(x, y, width, height);

    XEvent ev;
    for (;;) {
        XNextEvent(dpy, &ev);
        if (ev.type == Expose)
            continue;
        else if (ev.type == KeyPress && ev.xkey.keycode == exit_key)
            break;
        else if (ev.type == ButtonPress)
            toggle();
        else if (ev.type == FocusIn)
            highlight(True);
        else if (ev.type == FocusOut)
            highlight(False);
    }

    XCloseDisplay(dpy);
    return 0;
}
