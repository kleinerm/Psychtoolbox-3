function rval = kPsychNeedTripleWidthWindow
% rval = kPsychNeedTripleWidthWindow
%
% Return a flag that you can pass to the 'imagingmode' parameter of
% Screen('OpenWindow') in order to request use of triple-width windows.
% These onscreen windows are treated by all drawing functions (and geometry
% query functions like Screen('Rect', window); or Screen('WindowSize',
% window); as if they have three times the width of the underlying framebuffer in
% pixels.
%
% This is used for special packed-pixel display modes, where the content of
% three visual stimulus pixels is packed into one single output framebuffer
% pixel for video scanout. E.g., certain special medical displays for use
% in Radiology (cfe. "Eizo RadiForce GS-521") are able to display
% calibrated pure luminance images with a luminance precision of 8 bits
% per grayscale pixel at > 15 Megapixels resolution. These displays are driven
% over a DVI-D interface with a special pixel data transmission format:
% Three horizontally adjacent 8-bit luminance pixels are encoded into
% one 24-bit RGB output pixel. The monitor in "Subpixel drive" mode will
% output these three "color values" to the three luminance subpixels of
% a regular display pixel, thereby achieving three times the horizontal
% resolution of the video mode: 2560x2048 --> 7680x2048 ~ 15.6 Megapixels.
% To allow natural stimulus drawing our window needs to be three times the
% width of the output framebuffer, and the imaging pipeline will pack three
% stimulus pixels into the color channels of one output pixel.
%
% Usually user scripts won't pass this flag, but only special setup
% routines that are part of the Psychtoolbox itself.
%

rval = 2^25;
return
