xrectdraw
=========
A small program used to draw rectangles on X11. The idea came when I needed
something to indicate screen recordings for a small script using ffmpeg. I used
xrectsel for selecting the region for ffmpeg to capture, so I kinda got the
inspiration from there.

xrectdraw actually draws four windows making up the borders of the rectangle.
This means no compositor is needed, the root window isn't hogged and the region
is not overlaied.

Installation
------------
Edit config.mk to match your local setup (xrectdraw is installed into the
/usr/local namespace by default), then simply enter the following command to
install (if necessary as root):

    make clean install

Customization
-------------
xrectdraw can be customized by creating a custom config.h and (re)compiling the
source code.

Usage
-----
Together with the source code is a script called *screenrec.sh* which is a fine
example of how xrectdraw can be used. The program is used like this:

    xrectdraw 10 20 100 200 \#ff0000 3:2:3:2

The example above draws a rectangle, 100px wide, 200px heigh at the position
(x:10px, y:20px). The borders are red, the top and bottom are 3px wide and the
right and left are 2pc wide.

License
-------
The project is licensed under the MIT license.
