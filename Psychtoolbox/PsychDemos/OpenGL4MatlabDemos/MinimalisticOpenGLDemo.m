function MinimalisticOpenGLDemo
% MinimalisticOpenGLDemo - Demonstrate use of MATLAB-OpenGL toolbox
%
% This demo demonstrates use of OpenGL commands in a Matlab script to
% perform some very boring 3D rendering in Psychtoolbox.
%
% It shows a single static ball, lit with default lighting and exactly one
% light source. This is meant to demonstrate the minimum amount of code to
% draw anything visible with perspective projection. 
%
% Stop the demo by pressing any key.
%
% Notable implementation details:
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
% InitializeMatlabOpenGL(0,0) instead of InitializeMatlabOpenGL. This will
% disable automatic error-checking. You can then use gluErrorString to
% perform manual error-checks if you want.
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
% GPL license by Prof. Richard F. Murray, University of York, Canada.

%
% 15-Dec-2005 -- created (RFM)
% 21-Jan-2006 -- Modified for use with OpenGL-Psychtoolbox (MK)
% 16-Feb-2006 -- Modified for use with new MOGL (MK)
% 05-Mar-2006 -- Cleaned up for public consumption (MK)
% 19-Apr-2006 -- Derived from SpinningCubeDemo (MK)

% Is the script running in OpenGL Psychtoolbox?
AssertOpenGL;

% Find the screen to use for display:
screenid=max(Screen('Screens'));

% Setup Psychtoolbox for OpenGL 3D rendering support and initialize the
% mogl OpenGL for Matlab wrapper:
InitializeMatlabOpenGL;

% Open a double-buffered full-screen window on the main displays screen.
[win , winRect] = Screen('OpenWindow', screenid);

% Setup the OpenGL rendering context of the onscreen window for use by
% OpenGL wrapper. After this command, all following OpenGL commands will
% draw into the onscreen window 'win':
Screen('BeginOpenGL', win);

% Get the aspect ratio of the screen:
ar=winRect(4)/winRect(3);


% Setup default drawing color to yellow (R,G,B)=(1,1,0). This color only
% gets used when lighting is disabled - if you comment out the call to
% glEnable(GL.LIGHTING).
glColor3f(1,1,0);

% Turn on OpenGL local lighting model: The lighting model supported by
% OpenGL is a local Phong model with Gouraud shading.
% If you want to do any shape from shading studies, it is very important to
% understand the difference between a local lighting model and a global
% illumination model!!!
glEnable(GL.LIGHTING);

% Enable the first local light source GL.LIGHT_0. Each OpenGL
% implementation is guaranteed to support at least 8 light sources. 
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

% Point lightsource at position (x,y,z) == (1,2,3)...
glLightfv(GL.LIGHT0,GL.POSITION,[ 1 2 3 0 ]);

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
% proper occlusion handling:
glClear;
    
% Draw a predefined (built-in) object, the Utah teapot at a size of 0.5
% units.
glutSolidTeapot(0.5);

% Translate by +2 units in z-direction:
glTranslatef(0, 0, +2);

% Change the color - or better: The light reflection properties of the
% material of the following objects - to greenish. We only change ambient and
% diffuse reflection properties. The color for specular reflection is left
% to its default of "white":
glMaterialfv(GL.FRONT_AND_BACK,GL.AMBIENT, [ 0.0 0.6 0.0 1 ]);
glMaterialfv(GL.FRONT_AND_BACK,GL.DIFFUSE, [ 0.0 0.6 0.0 1 ]);

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
% line - by -90 degrees...
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

% Close onscreen window and release all other ressources:
Screen('CloseAll');

% Well done!
return
