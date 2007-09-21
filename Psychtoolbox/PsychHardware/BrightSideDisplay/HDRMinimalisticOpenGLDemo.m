function HDRMinimalisticOpenGLDemo
% HDRMinimalisticOpenGLDemo - Demonstrate use of MATLAB-OpenGL toolbox with
% the BrightSide-Technologies High Dynamic Range Display.
%
% It differs from the normal MinimalisticOpenGLDemo only by the OpenWindow
% call that requests BrightSide HDR support, and the setup of lightsource
% intensities to make better use of the large dynamic range of the HDR
% display.
%
% This demo demonstrates use of OpenGL commands in a Matlab script to
% perform some very boring 3D rendering in Psychtoolbox.
%
% It shows a single static ball, lit with default lighting and exactly one
% light source. This is meant to demonstrate the minimum amount of code to
% draw anything visible with perspective projection. It also draws a static
% teapot and some little box with a cone as roof.
%
% Then it waits for a keyboard press.
%
% After that it demonstrates how to do basic texture mapping and animation:
% It loads a JPEG image of the earths surface from the filesystem, using
% Matlabs imread() function, then converts the image into a Psychtoolbox
% texture using Screen('MakeTexture'), then provides this texture as a
% standard OpenGL compatible texture using Screen('GetOpenGLTexture').
% This standard texture is applied to a sphere using standard OpenGL commands
% and finally the sphere is drawn as a rotating object in a simple animation
% loop. --> You'll see a rotating earth.
%
% Stop the demo by pressing any key and it will finish.
%
% Notable implementation details regarding use of OpenGL:
%
% The call InitializeMatlabOpenGL at the top of the script initializes the
% Matlab-OpenGL toolbox and enables the 3D gfx support in Psychtoolbox to
% allow proper interfacing between the OpenGL toolbox and Psychtoolbox.
%
% After this call, all OpenGL functions are made available to Matlab with
% the same - or a very similar - calling syntax as in the C programming
% language. OpenGL constants are made available in a format that is optimized
% for Matlab, where the first underscore is replaced by a dot, e.g.,
% GL.DEPTH_TEST, instead of the C-style GL_DEPTH_TEST.
%
% In order to execute OpenGL 3D drawing commands to draw 3D stims into a
% Psychtoolbox Onscreen- or offscreen window, one needs to call
% Screen('BeginOpenGL', windowPtr). After OpenGL drawing and before
% execution of standard Screen() commands, one needs to call
% Screen('EndOpenGL', windowPtr) to tell Psychtoolbox that 3D drawing is
% finished.
%
% Some OpenGL functions that return complex parameters to Matlab are not
% yet implemented - this is work in progress. The performance will be also
% lower than when coding in a compiled language like C++ or C -- that's the
% Matlab tax you'll have to pay ;-)
%
% The toolbox checks after execution of each single OpenGL command if it
% caused some error. It aborts your script with an error message, if so. If
% you are happy with your code and want to disable these error checks in
% order to squeeze out a bit more speed, you can call
% InitializeMatlabOpenGL(0,0) instead of InitializeMatlabOpenGL at the top
% of your script. This will disable automatic error-checking. You can then
% use the commands gluErrorString or glGetError to perform manual error-checks
% in your code if you want.
%
% Apart from that, use of OpenGL for Matlab is the same as OpenGL for the C
% programming language. If you are used to OpenGL coding in C, it should be
% a zero effort transition to code in Matlab+PTB. If you don't know OpenGL
% then get yourself one of the many good books or visit one of the many
% OpenGL tutorials on the internet.
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
% The OpenGL for Matlab toolbox was developed and contributed under
% GPL license by Prof. Richard F. Murray, University of York, Canada and
% ported to M$-Windows and GNU/Linux and tuned by Mario Kleiner.
%
% The earth surface JPEG-image is taken from the Linux/KDE application
% kdeworldclock. kdeworldclock and its components are licensed under
% GPL. 

% 15-Dec-2005 -- created (RFM)
% 21-Jan-2006 -- Modified for use with OpenGL-Psychtoolbox (MK)
% 16-Feb-2006 -- Modified for use with new MOGL (MK)
% 05-Mar-2006 -- Cleaned up for public consumption (MK)
% 19-Apr-2006 -- Derived from SpinningCubeDemo (MK)
% 05-May-2006 -- Added some demo code for basic texture mapping (MK)

% Is the script running in OpenGL Psychtoolbox? Abort, if not.
AssertOpenGL;

% Find the screen to use for display:
screenid=max(Screen('Screens'));

% Setup Psychtoolbox for OpenGL 3D rendering support and initialize the
% mogl OpenGL for Matlab wrapper:
InitializeMatlabOpenGL;

% Open a double-buffered full-screen window on the main displays screen.
[win , winRect] = BrightSideHDR('OpenWindow', screenid);
% Setup the OpenGL rendering context of the onscreen window for use by
% OpenGL wrapper. After this command, all following OpenGL commands will
% draw into the onscreen window 'win':
Screen('BeginOpenGL', win);

% Disable Vertex color clamping in our MOGL OpenGL context. As the
% low-level GL call for doing this is currently not supported by MOGL, we
% use some helper function of the BrightSide driver:
BrightSideCore(5, 0);

% Get the aspect ratio of the screen:
ar=winRect(4)/winRect(3);
% Setup default drawing color to yellow (R,G,B)=(1000,1000,0). This color only
% gets used when lighting is disabled - if you comment out the call to
% glEnable(GL.LIGHTING).
%glColor3f(1000,1000,0);

% Turn on OpenGL local lighting model: The lighting model supported by
% OpenGL is a local Phong model with Gouraud shading. The color values
% at the vertices (corners) of polygons are computed with the Phong lighting
% model and linearly interpolated accross the inner area of the polygon from
% the vertex colors. The Phong lighting model is a coarse approximation of
% real world lighting with ambient light reflection (undirected isotropic light),
% diffuse light reflection (position wrt. light source matters, but observer
% position doesn't) and specular reflection (ideal mirror reflection for highlights).
%
% The model does not take any object relationships into account: Any effects
% of (self-)occlusion, (self-)shadowing or interreflection of light between
% objects are ignored. If you need shadows, interreflections and global illumination
% you will either have to learn advanced OpenGL rendering and shading techniques
% to implement your own realtime shadowing and lighting models, or
% compute parts of the scene offline in some gfx-package like Maya, Blender,
% Radiance or 3D Studio Max...
%
% If you want to do any shape from shading studies, it is very important to
% understand the difference between a local lighting model and a global
% illumination model!!!
glEnable(GL.LIGHTING);

% Enable the first local light source GL.LIGHT_0. Each OpenGL
% implementation is guaranteed to support at least 8 light sources,
% GL.LIGHT0, ..., GL.LIGHT7
glEnable(GL.LIGHT0);

% Enable proper occlusion handling via depth tests:
glEnable(GL.DEPTH_TEST);

% Set projection matrix: This defines a perspective projection,
% corresponding to the model of a pin-hole camera - which is a good
% approximation of the human eye and of standard real world cameras --
% well, the best aproximation one can do with 3 lines of code ;-)
glMatrixMode(GL.PROJECTION);
glLoadIdentity;

% Field of view is 25 degrees from line of sight. Objects closer than
% 0.1 distance units or farther away than 100 distance units get clipped
% away, aspect ratio is adapted to the monitors aspect ratio:
gluPerspective(25,1/ar,0.1,100);

% Setup modelview matrix: This defines the position, orientation and
% looking direction of the virtual camera:
glMatrixMode(GL.MODELVIEW);
glLoadIdentity;

% Our point lightsource is at position (x,y,z) == (1,2,3)...
glLightfv(GL.LIGHT0,GL.POSITION,[ 1 2 3 0 ]);
glLightfv(GL.LIGHT0,GL.AMBIENT,[ 10 10 10 0 ]);
glLightfv(GL.LIGHT0,GL.DIFFUSE,[ 100 100 100 0 ]);
glLightfv(GL.LIGHT0,GL.SPECULAR,[ 1000 1000 1000 0 ]);
glMaterialfv(GL.FRONT_AND_BACK,GL.SHININESS, 100);

% Cam is located at 3D position (3,3,5), points upright (0,1,0) and fixates
% at the origin (0,0,0) of the worlds coordinate system:
% The OpenGL coordinate system is a right-handed system as follows:
% Default origin is in the center of the display.
% Positive x-Axis points horizontally to the right.
% Positive y-Axis points vertically upwards.
% Positive z-Axis points to the observer, perpendicular to the display
% screens surface.
gluLookAt(3,3,5,0,0,0,0,1,0);

% Set background clear color to 'black' (R,G,B,A)=(0,0,0,0):
glClearColor(0,0,0,0);

% Clear out the backbuffer: This also cleans the depth-buffer for
% proper occlusion handling: You need to glClear the depth buffer whenever
% you redraw your scene, e.g., in an animation loop. Otherwise occlusion
% handling will screw up in funny ways...
glClear;
    
% Draw a predefined (built-in) object, the Utah teapot at a size of 0.5
% units.
glutSolidTeapot(0.5);

% Translate by +2 units in z-direction:
glTranslatef(0, 0, +2);

% Change the color - or better: The light reflection properties of the
% material - of the following objects to greenish. We only change ambient and
% diffuse reflection properties. The color for specular reflection is left
% to its default of "white":
glMaterialfv(GL.FRONT_AND_BACK,GL.AMBIENT, [ 0.0 0.600 0.0 1 ]);
glMaterialfv(GL.FRONT_AND_BACK,GL.DIFFUSE, [ 0.0 0.600 0.0 1 ]);

% Draw a solid sphere of radius 0.25
glutSolidSphere(0.25, 100, 100);

% From the position of the sphere, go 1.5 units into positive x-direction
% and -1 units back in z-direction:
glTranslatef(1.5, 0, -1);

% Change the color - or better: The light reflection properties of the
% material of the following objects - to blue. We only change ambient and
% diffuse reflection properties. The color for specular reflection is left
% to its default of "white":
glMaterialfv(GL.FRONT_AND_BACK,GL.AMBIENT, [ 0.0 0.0 1.0 1 ]);
glMaterialfv(GL.FRONT_AND_BACK,GL.DIFFUSE, [ 0.0 0.0 1.0 1 ]);

% Draw some solid cube:
glutSolidCube(0.25);

% Translate upwards (positve y-direction):
glTranslatef(0, 0.125, 0);

% Rotate our frame of reference - and thereby all objects drawn after this
% line - by -90 degrees around the current x-axis (1,0,0):
glRotatef(-90, 1, 0, 0);

% change material reflection properties again to red:
glMaterialfv(GL.FRONT_AND_BACK,GL.AMBIENT, [ 1.0 0.0 0.0 1 ]);
glMaterialfv(GL.FRONT_AND_BACK,GL.DIFFUSE, [ 1.0 0.0 0.0 1 ]);

% And draw some cone on top of the cube:
glutSolidCone(0.25, 0.25, 100, 100);

% Finish OpenGL rendering into PTB window. This will switch back to the
% standard 2D drawing functions of Screen and will check for OpenGL errors.
Screen('EndOpenGL', win);

% Show rendered image at next vertical retrace:
Screen('Flip', win);
    
% Wait for keyboard press.
KbWait;
% Wait for keyboard release:
while KbCheck; end;

% Now we draw a solid, spinning textured sphere of radius 1.0.

% Prepare texture to by applied to the sphere: Load & create it from an image file:
myimg = imread([PsychtoolboxRoot 'PsychDemos/OpenGL4MatlabDemos/earth_512by256.jpg']);
%myimg = double(myimg) * 0.01;

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
glDisable(GL.TEXTURE_2D);
glDisable(GL.TEXTURE_RECTANGLE_EXT);
glEnable(gltextarget);
% Bind our texture, so it gets applied to all following objects:
glBindTexture(gltextarget, gltex);

% Textures color texel values shall modulate the color computed by lighting model:
glTexEnvfv(GL.TEXTURE_ENV,GL.TEXTURE_ENV_MODE,GL.MODULATE);

% Clamping behaviour shall be a cyclic repeat:
glTexParameteri(gltextarget, GL.TEXTURE_WRAP_S, GL.REPEAT);
glTexParameteri(gltextarget, GL.TEXTURE_WRAP_T, GL.REPEAT);

% Set up minification and magnification filters. This is crucial for the thing to work!
glTexParameteri(gltextarget, GL.TEXTURE_MIN_FILTER, GL.LINEAR);
glTexParameteri(gltextarget, GL.TEXTURE_MAG_FILTER, GL.LINEAR);

% Set basic "color" of object to white to get a nice interaction between the texture
% and the objects lighting:
glMaterialfv(GL.FRONT_AND_BACK,GL.AMBIENT, [ .1 .1 .1 1 ]);
glMaterialfv(GL.FRONT_AND_BACK,GL.DIFFUSE, [ .4 .4 .4 1 ]);
glMaterialfv(GL.FRONT_AND_BACK,GL.SPECULAR, [ 1 1 1 1 ]);
glMaterialfv(GL.FRONT_AND_BACK,GL.SHININESS, 100);
% Reset our virtual camera and all geometric transformations:
glMatrixMode(GL.MODELVIEW);
glLoadIdentity;

% Reposition camera (see above):
gluLookAt(0,0,5,0,0,0,0,1,0);

% Create the sphere as a quadric object. This is needed because the simple glutSolidSphere
% command does not automatically assign texture coordinates for texture mapping onto a sphere:
% mysphere is a handle that you need to pass to all quadric functions:
mysphere = gluNewQuadric;

% Enable automatic generation of texture coordinates for our quadric object:
gluQuadricTexture(mysphere, GL.TRUE); 

% Apply some static rotation to the object to have a nice view onto it:
% This basically rotates our spinning earth into an orientation that
% roughly matches the real orientation in space...
% First -90 degrees around its x-axis...
glRotatef(-90, 1,0,0);

% ...then 18 degrees around its new (rotated) y-axis...
glRotatef(18,0,1,0);

% Now for our little animation loop. This loop will run until a key is pressed.
% It rotates the object by a few degrees (actually: Applies a rotation transformation
% to all objects to be drawn) and then redraws it at its new orientation:
while ~KbCheck
  % Clear out backbuffer and depth buffer:
  glClear;

  % Increment rotation angle around new z-Axis (0,0,1)  by 0.1 degrees:
  glRotatef(0.1, 0, 0, 1);

  % Draw the textured sphere-quadric of radius 0.7. As OpenGL has to approximate
  % all curved surfaces (i.e. spheres) with flat triangles, we tell it to resolve
  % the sphere into 100 slices in elevation and 100 sectors in azimuth: Higher values
  % provide a better approximation, but they take longer to draw. Live is full of
  % trade-offs...
  gluSphere(mysphere, 0.7, 1000, 1000);

  % Finish OpenGL rendering into PTB window. This will switch back to the
  % standard 2D drawing functions of Screen and will check for OpenGL errors.
  Screen('EndOpenGL', win);

  % Show new image at next retrace:
  Screen('Flip', win);

  % Start OpenGL rendering again after flip for drawing of next frame...
  Screen('BeginOpenGL', win);

  % Ready for next draw loop iteration...
end;

% Done with the drawing loop:

% Delete our sphere object:
gluDeleteQuadric(mysphere);

% Unselect our texture...
glBindTexture(gltextarget, 0);

% ... and disable texture mapping:
glDisable(gltextarget);

% End of OpenGL rendering...
Screen('EndOpenGL', win);

% Close onscreen window and release all other ressources:
Screen('CloseAll');

% Well done!
return
