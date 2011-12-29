function rval = kPsychNeedTwiceWidthWindow
% rval = kPsychNeedTwiceWidthWindow
%
% Return a flag that you can pass to the 'imagingmode' parameter of
% Screen('OpenWindow') in order to request use of twice-width windows.
% These onscreen windows are treated by all drawing functions (and geometry
% query functions like Screen('Rect', window); or Screen('WindowSize',
% window); as if they have twice the width of the underlying framebuffer in
% pixels.
%
% This is used for special packed-pixel display modes, where the content of
% two visual stimulus pixels is packed into one single output framebuffer
% pixel for video scanout. E.g., certain special medical displays for use
% in Radiology (cfe. "Eizo RadiForce GS-520") are able to display
% calibrated pure luminance images with a luminance precision of 12 bits
% per grayscale pixel at 5 Megapixels resolution. These displays are driven
% over a single-link DVI-D interface with a special pixel data transmission
% format: Two horizontally adjacent 12-bit luminance pixels are encoded
% into one 24-bit RGB pseudo-color pixel. This requires a framebuffer
% encoding two luminance pixels in a single RGB8 output pixel, ie. the
% stimulus image in its "natural" representation is twice as wide as the
% underlying scanout framebuffer of the stimulus window. For such use cases
% we need the ability to have twice-width windows.
%
% Usually user scripts won't pass this flag, but only special setup
% routines that are part of the Psychtoolbox itself.
%

rval = 2^16;
return
