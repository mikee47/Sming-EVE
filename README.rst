Sming EVE library
=================

This library is primarily to support FT813 display driver used with
`Newhaven NHD-5.0-800480TF-ATXL#-CTP <https://www.newhavendisplay.com/nhd50800480tfatxlctp-p-6062.html>`__
display. These are 5" 800x480 pixel multi-touch capacitive displays and require a display driver.

Initial evaluation used the :library:`S1D13781 <TFT_S1D13781>` which is a traditional SPI raster driver capable of driving these displays, albeit with a lower colour resolution. The host CPU has to do the work of updating display regions.

The FTDI/Bridgetek EVE (Embedded Video Engine) chips offload most of the work from the host CPU.
The contents of the screen are described using high-level primitives (lines, points, bitmaps) which the controller then executes. It also contains a co-processor which supports widget controls, MJPEG video playback, audio streaming. The device contains 1MiB of RAM for application use, such as custom fonts, bitmaps, media FIFO, etc.

The goal is to provide an interface to these displays via the Sming :library:`Graphics` library.


Development
-----------

The library is being developed on a custom board containing an ESP32-S2 WROOM module, USB OTG, SD card and loudspeaker. The EVE connects to the esp32 via Quad-SPI interface at 25MHz. Using asynchronous updates and DMA, glitch-free displays of 60fps are easily achieved with minimal CPU loading. The esp32 is therefore free to perform other functions: streaming data over WiFi, decoding MP3/FLAC audio files, signal processing, etc.

These are the sorts of applications in mind:

- Digital signal analyser, oscilloscope. Additional interface circuits will be necessary for these to handle signal conditioning, but that could be as simple as some input protection circuitry.
- Media player to stream high-quality audio from a network server or the SD card to an external USB DAC.
- Energy management interface. Wall-mounted control for managing solar battery storage, heating, etc.

Whilst linux servers, mobile phones and tablets can provide useful stand-ins, there are many situations where a fixed-function solution is preferable.


Goals
-----

The :library:`HardwareSPI` library provides asynchronous DMA support for updates.
Taking advantage of this requires that data be available in RAM, and that the size of each transfer is reasonably large.
Generally, therefore, the (co-processor) command list is pre-constructed first in CPU RAM, then dispatched to the display.

Constructing a scene using the Graphics library involves constructing a :cpp:class:`Scene` containing a list of high-level objects. A scene is typically used to update only a region of the screen, rather than the entire display. A separate render queue is used to manage these updates.

For EVE we can use one scene to describe everything. A second scene can be used to prepare the next frame if faster updates are required.

We'd probably need to extend or re-implement Graphics::CommandList for EVE.

For example, CMD_LOADIMAGE is followed by actual image data. We might output this as follows::

    +0  Code::command
    +1  12 (length)
    +4  CMD_LOADIMAGE
    +8  ptr
    +12 options
    +16 Code::writeDataBuffer
    +20 1234 (length)
    +24 image data (pointer to RAM buffer)

When SPI transaction is complete the callback would handle re-loading the RAM buffer with further data, for example if it's from a stream.
Internally we'd track space in the EVE command FIFO. When it's full we'd hold-off until our ISR fires, which then triggers a callback which queues a 'read status' operation, etc.

With previous displays we need a lot of interrupt processing but that can be much reduced for EVE.
It would still be useful to queue multiple commands to the display list to read multiple registers, for example, which then triggers an application callback to process the data as a set.
We can use the existing Code::read mechanisms to handle this within our command lists.

The optimal solution may be to do all this work in the renderer, the command list being quite simple.
We can track FIFO position (REG_CMD_WRITE) internally which would avoid the need to read it from the device.
Having that available would allow more efficient command list generation, e.g. working with CMD_GETPTR result.

Low-level user API is via Surface so that's where the work gets done.

How to implement scrolling?
How to modify or add to display list? Could copy to GRAM but then how to modify?

Maybe with both of these the easiest option is to manipulate a scene, so we effectively re-draw it each time.
Same technique for animation.
Assets stored in GRAM so only have to send those once.
Built-in animations (SPINNER, SCREENSAVER, SKETCH) use CMD_MACRO_0 & 1 so these look useful for other stuff.

Q. What's the max. frame rate?
A. Display-dependent, NHD5 is 928*525@30MHz = about 61.58 frames/sec.


Python support
--------------

Building command lists in python then sending them to the dev. board is useful for evaluation.

Display-list commands are already well-defined since each entry is one 32-bit word. The co-processor commands are more complex though, so using python to generate the C++ wrappers for generating efficient inline code is useful.

An integration for the *graphical editor* could support generation of these display lists directly from the scene descriptions.


References
----------

Bridgetek
    There are three Windows tools which are quite useful https://brtchip.com/toolchains/ and (mostly) run in linux using Wine. These are completely closed-source though.

EVE code library
    https://github.com/RudolphRiedel/FT800-FT813

Gameduino
    These devices are used in the `Gameduino <https://excamera.com/sphinx/gameduino/>`__.
    The author has an `open source library <https://github.com/jamesbowman/gd2-lib>`__.

