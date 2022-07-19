# video-splitter
Video splitter command line tool to split videos by count or seconds.

# How it works?
This tool uses the [FFmpeg](http://ffmpeg.org/) tool to process videos.
So if you want to use this tool, you must add FFmpeg executable to you path.

# Usage
```
video-splitter [options] video-file.mp4

Options:
-c    count    video count to generate (ignores -s parameter)
-s    seconds  time for every video (default 30)
```

# Example
```
video-splitter -s 15 video.mp4
```

# License
The Unlicense. Feel free to use or change it how you need.
