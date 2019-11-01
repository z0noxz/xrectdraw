#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>

Display *dpy;
int screen, gfocus;
Window root;
Window win[4];
Drawable drw;
XColor color;
XColor color_dim;

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
    int i;
    XWindowAttributes wa;
    XSetWindowAttributes swa;
    XClassHint ch = {"xdrwrct", "xdrwrct"};
    XIM xim;
    XIC xic;

    int ix = x - 2,
        iy = y - 2,
        iw = 2,
        ih = height + 5;

    swa.border_pixel = 0;
    swa.background_pixel = color.pixel;
    swa.override_redirect = 1;
    swa.event_mask = ExposureMask | KeyPressMask | ButtonPressMask | FocusChangeMask;

    XGetWindowAttributes(dpy, root, &wa);

    for (i = 0; i < 4; i++) {
        win[i] = XCreateWindow(dpy, root, ix, iy, iw, ih, 0,
            CopyFromParent, CopyFromParent, CopyFromParent,
            CWOverrideRedirect | CWBackPixel | CWEventMask, &swa);

        XSetClassHint(dpy, win[i], &ch);

        xim = XOpenIM(dpy, NULL, NULL, NULL);
        xic = XCreateIC(xim, XNInputStyle, XIMPreeditNothing | XIMStatusNothing,
            XNClientWindow, win[i], XNFocusWindow, win[i], NULL);

        XMapRaised(dpy, win[i]);
        XSetInputFocus(dpy, win[i], RevertToParent, CurrentTime);

        switch (i) {
        case 0:
            ix += (width + 3);
            break;
        case 1:
            ix -= (width + 3);
            iw = (width + 5);
            ih = 2;
            break;
        case 2:
            ih = 5;
            iy += (height + 3);
            break;
        }
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
}

void
setcolor(const char *clr)
{
    int i, r;
    const char *ptr;

    color.flags = DoRed | DoGreen | DoBlue;
    color_dim.flags = DoRed | DoGreen | DoBlue;
    for (i = 0; i < 3; i++) {
        ptr = clr + (1 + i * 2);
        r = (hex(ptr[0]) * 16 + hex(ptr[1])) * 256;

        switch (i) {
        case 0:
            color.red = r;
            color_dim.red = r * 0.4;
            break;
        case 1:
            color.green = r;
            color_dim.green = r * 0.4;
            break;
        case 2:
            color.blue = r;
            color_dim.blue = r * 0.4;
            break;
        }
    }
    XAllocColor(dpy, DefaultColormap(dpy, screen), &color);
    XAllocColor(dpy, DefaultColormap(dpy, screen), &color_dim);
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
    unsigned long pixel = (gfocus = focus) ? color.pixel : color_dim.pixel;

    for (int i = 0; i < 4; i++) {
        XSetWindowBackground(dpy, win[i], pixel);
        XClearArea(dpy, win[i], 0, 0, 0, 0, True);
    }
}

int
main(int argc, char *argv[])
{
    int width, height,
        x, y;

    if (argc != 6
        || strcmp(argv[1], "-h") == 0
        || strcmp(argv[1], "--help") == 0) {
        die("usage: %s x y width height #RRGGBB\n", argv[0]);
        return 1;
    }

    if (!(dpy = XOpenDisplay(NULL)))
        puts("error: cannot open display");
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
        else if (ev.type == KeyPress && ev.xkey.keycode == 0x09)
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
