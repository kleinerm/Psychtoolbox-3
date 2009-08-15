function CylinderAnnulusOpenGLDemo(patternType, multiSample)
% CylinderAnnulusOpenGLDemo([patternType=0][, multiSample=0])
%
% This demo demonstrates use of OpenGL commands in a Matlab script to
% map a 2D image onto a 3D cylindrical surface.
%
% It loads a JPEG image of the earths surface from the filesystem, using
% Matlabs imread() function, then converts the image into a Psychtoolbox
% texture using Screen('MakeTexture'), then provides this texture as a
% standard OpenGL compatible texture using Screen('GetOpenGLTexture').
% This standard texture is applied to a cylinder using standard OpenGL commands
% and finally the cylinder is drawn as a rotating object in a simple animation
% loop. --> You'll see a rotating cylinder.
%
% Stop the demo by pressing any key and it will finish.
%
% The optional parameter 'multiSample' allows to enable anti-aliased
% drawing with 'multiSample' samples per pixel on hardware that supports
% this.
%
% The optional parameter 'patternType' allows (if set to non-zero
% value) to apply a specific pattern to the spinning cylinder, instead of
% a "earth surface texture image". This demonstrates algorithmic texture
% generation and the use of trilinear mipmap filtering to improve image
% quality for high frequency edges and such...
%
% 0 = Jpeg image of earth surface.
% 1 = Checkerboard pattern.
% 2 = Simple vertical annulus.
%
% The OpenGL Red Book is a great introduction and reference for OpenGL
% programming. Release 1.0 is available online, later releases can be
% purchased in any good book store:
%
% http://www.opengl.org/documentation/red_book_1.0/
%
% For more infos, code samples, tutorials, online documentation, go to:
%
% http://www.opengl.org
%
% The earth surface JPEG-image is taken from the Linux/KDE application
% kdeworldclock. kdeworldclock and its components are licensed under
% GPL.

% History:
% 09-Aug-2009 -- Derived from MinimalisticOpenGLDemo (MK)

if nargin < 2
    multiSample = [];
end

if nargin < 1
    patternType = [];
end

if isempty(patternType)
    patternType = 0;
end

% Is the script running in OpenGL Psychtoolbox? Abort, if not.
AssertOpenGL;

% Find the screen to use for display:
screenid=max(Screen('Screens'));

% Setup Psychtoolbox for OpenGL 3D rendering support and initialize the
% mogl OpenGL for Matlab wrapper:
InitializeMatlabOpenGL;

% Open a double-buffered full-screen window on the main displays screen.
[win, winRect] = Screen('OpenWindow', screenid, 0, [], [], [], [], multiSample);

% Setup the OpenGL rendering context of the onscreen window for use by
% OpenGL wrapper. After this command, all following OpenGL commands will
% draw into the onscreen window 'win':
Screen('BeginOpenGL', win);

% Get the aspect ratio of the screen:
ar=winRect(4)/winRect(3);

% Setup default drawing color to white: This is the base color of the
% cylinders projection, which is modulated in intensity or color by the
% texture pattern:
glColor3f(1,1,1);

% Enable proper (self-)occlusion handling via depth tests:
glEnable(GL.DEPTH_TEST);

% Set projection matrix: This defines a perspective projection,
% corresponding to the model of a pin-hole camera - which is a good
% approximation of the human eye and of standard real world cameras --
% well, the best aproximation one can do with 3 lines of code ;-)
glMatrixMode(GL.PROJECTION);
glLoadIdentity;

% Field of view is 25 degrees from line of sight. Objects closer than
% 0.1 distance units or farther away than 100 distance units get clipped
% away and not drawn. Aspect ratio is adapted to the monitors aspect ratio:
gluPerspective(25, 1/ar, 0.1, 100);

% Setup modelview matrix: This defines the position, orientation and
% looking direction of the virtual camera or virtual projector:
glMatrixMode(GL.MODELVIEW);
glLoadIdentity;

% Cam/Projector is located at 3D position (0,0,15), points upright (0,1,0) and fixates
% at the origin (0,0,0) of the worlds coordinate system:
%
% The OpenGL coordinate system is a right-handed system as follows:
% Default origin is in the center of the display.
% Positive x-Axis points horizontally to the right.
% Positive y-Axis points vertically upwards.
% Positive z-Axis points to the observer, perpendicular to the display
% screens surface.
gluLookAt(0, 0, 15, 0, 0, 0, 0, 1, 0);

% Set background clear color to 'black' (R,G,B,A)=(0,0,0,0):
glClearColor(0,0,0,0);

% Clear out the backbuffer: This also cleans the depth-buffer for
% proper occlusion handling: You need to glClear the depth buffer whenever
% you redraw your scene, e.g., in an animation loop. Otherwise occlusion
% handling will screw up in funny ways...
glClear;

% Finish OpenGL rendering into PTB window. This will switch back to the
% standard 2D drawing functions of Screen and will check for OpenGL errors.
Screen('EndOpenGL', win);

switch (patternType)
    case 0,
        % Prepare texture to by applied to the sphere: Load & create it from an image file:
        myimg = imread([PsychtoolboxRoot 'PsychDemos/OpenGL4MatlabDemos/earth_512by256.jpg']);
    case 1,
        % Apply regular checkerboard pattern as texture:
        bv = zeros(16);
        wv = ones(16);
        myimg = double(repmat([bv wv; wv bv],32,32) > 0.5) * 255;
    case 2,
        % Apply an annulus pattern as texture:
        % Everything white...
        myimg = ones(1024, 1) * 255;
        % ...except for a vertical stripe along the cylinder of width 100
        % texels, which we color in a nice 50% intensity of 128 instead
        % 255.
        myimg(1:100, :) = 128;
    otherwise
        sca;
        error('Unknown pattern type!');
end

% Make a special power-of-two texture from the image by setting the enforcepot - flag to 1
% when calling 'MakeTexture'. GL_TEXTURE_2D textures (==power of two textures) are
% especially easy to handle in OpenGL. If you use the enforcepot flag, it is important
% that the texture image 'myimg' has a width and a height that is exactly a power of two,
% otherwise this command will fail: Allowed values for image width and height are, e.g.,
% 1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048 and on some high-end gfx cards
% 4096 pixels. Our example image has a size of 512 by 256 pixels...
% Psychtoolbox also supports rectangular textures of arbitrary size, so called
% GL_TEXTURE_RECTANGLE_2D textures. These are normally used for Screen's drawing
% commands, but they are more difficult to handle in standard OpenGL code...
mytex = Screen('MakeTexture', win, myimg, [], 1);

% Retrieve OpenGL handles to the PTB texture. These are needed to use the texture
% from "normal" OpenGL code:
[gltex, gltextarget] = Screen('GetOpenGLTexture', win, mytex);

% Begin OpenGL rendering into onscreen window again:
Screen('BeginOpenGL', win);

% Enable texture mapping for this type of textures...
glEnable(gltextarget);

% Bind our texture, so it gets applied to all following objects:
glBindTexture(gltextarget, gltex);

% Textures color texel values shall modulate the color computed by lighting model:
glTexEnvfv(GL.TEXTURE_ENV, GL.TEXTURE_ENV_MODE, GL.MODULATE);

% Clamping behaviour shall be a cyclic repeat:
glTexParameteri(gltextarget, GL.TEXTURE_WRAP_S, GL.REPEAT);
glTexParameteri(gltextarget, GL.TEXTURE_WRAP_T, GL.REPEAT);

% Set up minification and magnification filters. This is crucial for the thing to work!
% We use trilinear mipmap filtering for the minification filter, so we
% don't get too many aliasing artifacts for patterns with high-frequency edges.
glTexParameteri(gltextarget, GL.TEXTURE_MIN_FILTER, GL.LINEAR_MIPMAP_LINEAR);

% Need mipmapping for trilinear filtering --> Create mipmaps:
if ~isempty(findstr(glGetString(GL.EXTENSIONS), 'GL_EXT_framebuffer_object'))
    % Ask the hardware to generate all depth levels automatically:
    glGenerateMipmapEXT(GL.TEXTURE_2D);
else
    % No hardware support for fast auto-mipmap-generation. Do it "manually":

    % Use GLU to compute the image resolution mipmap pyramid and create
    % OpenGL textures ouf of it: This is slow, compared to glGenerateMipmapEXT:
    r = gluBuild2DMipmaps(gltextarget, GL.LUMINANCE, size(myimg,1), size(myimg,2), GL.LUMINANCE, GL.UNSIGNED_BYTE, uint8(myimg));
    if r>0
        sca;
        error('gluBuild2DMipmaps failed for some reason.');
    end
end

% Use bilinear filtering for magnification filter:
glTexParameteri(gltextarget, GL.TEXTURE_MAG_FILTER, GL.LINEAR);

% Create the cylinder as a quadric object. 'mycylinder' is a handle that you
% need to pass to all quadric functions:
mycylinder = gluNewQuadric;

% Enable automatic generation of texture coordinates for our quadric object:
gluQuadricTexture(mycylinder, GL.TRUE);

% Apply some static rotation to the object to have a nice view onto it:
glRotatef(0.0, 1,0,0);  % Rotate around x axis.
glRotatef(0.0, 0,1,0);  % Rotate around y axis.

% Now for our little animation loop. This loop will run until a key is
% pressed. It rotates the object by a few degrees (actually: Applies a
% rotation transformation to all objects to be drawn) and then redraws it
% at its new orientation:
while ~KbCheck
    % Clear out backbuffer and depth buffer:
    glClear;

    % Increment rotation angle around z-Axis (0,0,1) by 0.1 degrees:
    glRotatef(0.1, 0, 0, 1);

    % Draw a textured cylinder with a radius of 1.0 units at base and top,
    % height of 10.0 units. Divide the cylinder into 360 slices around
    % z-axis, ie., 1 degree resolution, and 1 stack along the z-axis. Because
    % graphics hardware can only render flat surfaces, the curved cylinder is
    % approximated by a mesh of 360 rectangular panels, arranged to form a
    % cylinder. Increasing the subdivision beyound 360 may create a better
    % approximation at the cost of slower drawing speeds:
    gluCylinder(mycylinder, 1.0, 1.0, 10.0, 360, 1);

    % Finish OpenGL rendering into PTB window. This will switch back to the
    % standard 2D drawing functions of Screen and will check for OpenGL errors.
    Screen('EndOpenGL', win);

    % Show new image at next retrace: The special dontclear = 2 flag
    % prevents 'Flip' from clearing the backbuffer, as this is done at the
    % top of the while-loop with glClear anyway:
    Screen('Flip', win, 0, 2);

    % Start OpenGL rendering again after flip for drawing of next frame...
    Screen('BeginOpenGL', win);

    % Ready for next draw loop iteration...
end;

% Done with the drawing loop: Shutdown stuff...

% Delete our cylinder object:
gluDeleteQuadric(mycylinder);

% Unselect our texture...
glBindTexture(gltextarget, 0);

% ... and disable texture mapping:
glDisable(gltextarget);

% End of OpenGL rendering...
Screen('EndOpenGL', win);

% Close onscreen window and release all other ressources:
Screen('CloseAll');

% Well done!
return;
