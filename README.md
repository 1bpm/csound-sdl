## Introduction
csound-sdl provides a simple interface for (currently quite) low level SDL calls. At current only basic shapes are available in order to investigate the possibilities and efficiency of using SDL from within Csound. An example is provided in the examples directory which utilises all current opcodes.


## Requirements
 - Csound libraries
 - Cmake >= 2.8.12
 - LibSDL2


## Building
Create a build directory at the top of the source tree, execute *cmake ..*, *make* and optionally *make install* as root. If the latter is not used/possible then the resulting module can be used with the *--opcode-lib* flag in Csound. eg:
	
	cd csound-sdl
	mkdir build && cd build
	cmake ..
	make && sudo make install


## Opcode overview
### sdlinit
*ihandle, kmousetrigger, kmousex, kmousey sdlinit Swindowname, iwidth, iheight, ifps*

Initialise the SDL window.

 - ihandle: the handle to be used by further SDL opcodes to write to that window
 - kmousetrigger: fires when mouse button is clicked in the window
 - kmousex: reports mouse X position in window, normalised from 0 to 1
 - kmousey: reports mouse Y position in window, normalised from 0 to 1

 - Swindowname: the title given to the window
 - iwidth: width of window in pixels
 - iheight: height of window in pixels
 - ifps: frames per second of rendering

### sdlcolour
*ihandle sdlcolour kred, kgreen, kblue, kalpha*

Set a colour to be used in later drawing.

 - ihandle: the handle for the colour to be used by further SDL opcodes
 
 - kred: red amount with range 0 to 255
 - kgreen: green amount with range 0 to 255
 - kblue: blue amount with range 0 to 255
 - kalpha: alpha amount with range 0 to 255

### sdlrect
*sdlrect iwindow, icolour, kx, ky, kwidth, kheight*

Create a rectangle.

 - iwindow: the window handle from sdlinit
 - icolour: the colour handle from sdlcolour
 - kx: X position
 - ky: Y position
 - kwidth: width
 - kheight: height

## sdlline
*sdlline iwindow, icolour, kx1, ky1, kx2, ky2*

Create a line.

 - iwindow: the window handle from sdlinit
 - icolour: the colour handle from sdlcolour
 - kx1: X position of start point
 - ky1: Y position of start point
 - kx2: X position of end point
 - ky2: Y position of end point

