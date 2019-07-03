# FFmpeg-foobar README
This is an modified version of ffmpeg that is added two filters: vf\_foobar and af\_volumedetect
* vf\_foobar: does object detect and submit the detect information to a server by calling /home/zwz/collector/post-data-fire.py, which belongs to another project of post-data-borard. The main file is libavfilter/vf\_foobar.c. It has two modes: normal mode and autotrigger mode. the command line is: ffmpeg -i /dev/video0 -vf foobar=mode=0 ... to determine the mode. It relies on libmymatcher, which relies on opencv. See libmymatcher/readme.txt for more information. The compile\_and\_install.sh compiles the libavfilter.so with libmymatcher.

* af\_volumedetect: just to detect the volume and if it reaches a threshold, do something... the file is at libavfilter/af\_volumedetect. 

Recommended configure: disable static, enable shared

FFmpeg README
=============

FFmpeg is a collection of libraries and tools to process multimedia content
such as audio, video, subtitles and related metadata.

## Libraries

* `libavcodec` provides implementation of a wider range of codecs.
* `libavformat` implements streaming protocols, container formats and basic I/O access.
* `libavutil` includes hashers, decompressors and miscellaneous utility functions.
* `libavfilter` provides a mean to alter decoded Audio and Video through chain of filters.
* `libavdevice` provides an abstraction to access capture and playback devices.
* `libswresample` implements audio mixing and resampling routines.
* `libswscale` implements color conversion and scaling routines.

## Tools

* [ffmpeg](https://ffmpeg.org/ffmpeg.html) is a command line toolbox to
  manipulate, convert and stream multimedia content.
* [ffplay](https://ffmpeg.org/ffplay.html) is a minimalistic multimedia player.
* [ffprobe](https://ffmpeg.org/ffprobe.html) is a simple analysis tool to inspect
  multimedia content.
* Additional small tools such as `aviocat`, `ismindex` and `qt-faststart`.

## Documentation

The offline documentation is available in the **doc/** directory.

The online documentation is available in the main [website](https://ffmpeg.org)
and in the [wiki](https://trac.ffmpeg.org).

### Examples

Coding examples are available in the **doc/examples** directory.

## License

FFmpeg codebase is mainly LGPL-licensed with optional components licensed under
GPL. Please refer to the LICENSE file for detailed information.

## Contributing

Patches should be submitted to the ffmpeg-devel mailing list using
`git format-patch` or `git send-email`. Github pull requests should be
avoided because they are not part of our review process and will be ignored.
