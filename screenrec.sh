#!/bin/sh

# use xrectsel to define region
geo=$(xrectsel "%x %y %w %h")

# record region on screen using backgrounded ffmpeg
ffmpeg                                                                      \
    -video_size $(echo $geo | awk {'print $3 "x" $4'})                      \
    -framerate 25                                                           \
    -f x11grab                                                              \
    -i $(echo $geo | awk {'print ":0.0+" $1 "," $2'})                       \
    -y output.mkv &

# store ffmpeg pid
ffmpeg_pid=$!

# run xrectdraw to indicate region being recorded
xrectdraw $geo \#ff0000

# when xrectdraw dies kill ffmpeg
kill $ffmpeg_pid
