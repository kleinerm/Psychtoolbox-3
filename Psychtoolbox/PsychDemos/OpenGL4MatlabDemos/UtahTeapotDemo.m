function UtahTeapotDemo
% UtahTeapotDemo - Demonstrate use of MATLAB-OpenGL toolbox
%
% This demo demonstrates use of OpenGL commands in a Matlab script to
% perform some 3D animation in Psychtoolbox.
% It shows a randomly spinning, lit teapot, the so called "Utah Teapot".
%
% Stop the demo by pressing any key.
%
% Notable implementation details:
% The call InitializeMatlabOpenGL(1) at the top of the script initializes the
% Matlab-OpenGL toolbox and enables the 3D gfx support in Psychtoolbox to
% allow proper interfacing between the OpenGL toolbox and Psychtoolbox.
%
% After this call, all OpenGL functions are made available to Matlab with
% the same - or a very similar - calling syntax as in the C programming
% language. OpenGL constants are made available in C-Style, e.g.,
% GL_DEPTH_TEST, and in a format that is optimized for Matlab, where the
% first underscore is replaced by a dot, e.g., GL.DEPTH_TEST. The former
% style is more convenient if you want to copy & paste OpenGL code written
% in C into a Matlab M-File for use, but it only works if you put all your
% code into one single M-File or function. The second style works in
% subfunctions as well, if you place the commands "global GL" and "global
% GLU" at the top of each function... This inconvenience is unavoidable due
% to the design of Matlab.
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

% Is the script running in OpenGL Psychtoolbox?
AssertOpenGL;

% Find the screen to use for display:
screenid=max(Screen('Screens'));

% Disable Synctests for this simple demo:
Screen('Preference','SkipSyncTests',1);

% Setup Psychtoolbox for OpenGL 3D rendering support and initialize the
% mogl OpenGL for Matlab wrapper:
InitializeMatlabOpenGL(1);

% Open a double-buffered full-screen window on the main displays screen.
[win , winRect] = Screen('OpenWindow', screenid);

% Setup the OpenGL rendering context of the onscreen window for use by
% OpenGL wrapper. After this command, all following OpenGL commands will
% draw into the onscreen window 'win':
Screen('BeginOpenGL', win);

% Get the aspect ratio of the screen:
ar=winRect(4)/winRect(3);

% Turn on OpenGL local lighting model: The lighting model supported by
% OpenGL is a local Phong model with Gouraud shading.
glEnable(GL_LIGHTING);

% Enable the first local light source GL_LIGHT_0. Each OpenGL
% implementation is guaranteed to support at least 8 light sources. 
glEnable(GL_LIGHT0);

% Enable two-sided lighting - Back sides of polygons are lit as well.
glLightModelfv(GL_LIGHT_MODEL_TWO_SIDE,GL_TRUE);

% Enable proper occlusion handling via depth tests:
glEnable(GL_DEPTH_TEST);

% Define the cubes light reflection properties by setting up reflection
% coefficients for ambient, diffuse and specular reflection:
glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT, [ 1 1 1 1 ]);
glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE, [ .78 .57 .11 1 ]);
glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR, [ 1 1 1 1 ]);
glMaterialfv(GL_FRONT_AND_BACK,GL_SHININESS,128);

% Set projection matrix: This defines a perspective projection,
% corresponding to the model of a pin-hole camera - which is a good
% approximation of the human eye and of standard real world cameras --
% well, the best aproximation one can do with 3 lines of code ;-)
glMatrixMode(GL_PROJECTION);
glLoadIdentity;

% Field of view is +/- 25 degrees from line of sight. Objects close than
% 0.1 distance units or farther away than 100 distance units get clipped
% away, aspect ratio is adapted to the monitors aspect ratio:
gluPerspective(25,1/ar,0.1,100);

% Setup modelview matrix: This defines the position, orientation and
% looking direction of the virtual camera:
glMatrixMode(GL_MODELVIEW);
glLoadIdentity;

% Cam is located at 3D position (0,0,10), points upright (0,1,0) and fixates
% at the origin (0,0,0) of the worlds coordinate system:
gluLookAt(0,0,10,0,0,0,0,1,0);

% Setup position and emission properties of the light source:

% Set background color to 'black':
glClearColor(0,0,0,0);

% Point lightsource at (1,2,3)...
glLightfv(GL_LIGHT0,GL_POSITION,[ 0 0 -100 0 ]);

% Emits white (1,1,1,1) diffuse light:
glLightfv(GL_LIGHT0,GL_DIFFUSE, [ 1 1 1 1 ]);

% Emits reddish (1,1,1,1) specular light:
glLightfv(GL_LIGHT0,GL_SPECULAR, [ 1 0 0 1 ]);

% There's also some blue, but weak (R,G,B) = (0.1, 0.1, 0.1)
% ambient light present:
glLightfv(GL_LIGHT0,GL_AMBIENT, [ .1 .1 .6 1 ]);

% Initialize amount and direction of rotation
theta=0;
rotatev=[ 0 0 1 ];

% Animation loop: Run until key press...
while (1)
    % Calculate rotation angle for next frame:
    theta=mod(theta+0.3,360);
    rotatev=rotatev+0.1*[ sin((pi/180)*theta) sin((pi/180)*2*theta) sin((pi/180)*theta/5) ];
    rotatev=rotatev/sqrt(sum(rotatev.^2));

    % Setup cubes rotation around axis:
    glPushMatrix;
    glRotated(theta,rotatev(1),rotatev(2),rotatev(3));

    % Clear out the backbuffer: This also cleans the depth-buffer for
    % proper occlusion handling:
    glClear;
    
    glutSolidTeapot(1.0);

    glPopMatrix;
    
    % Finish OpenGL rendering into PTB window and check for OpenGL errors.
    Screen('EndOpenGL', win);

    % Show rendered image at next vertical retrace:
    Screen('Flip', win);

    % Switch to OpenGL rendering again for drawing of next frame:
    Screen('BeginOpenGL', win);
    
    % Check for keyboard press and exit, if so:
    if KbCheck
        break;
    end;
end

% Shut down OpenGL rendering:
Screen('EndOpenGL', win);

% Close onscreen window and release all other ressources:
Screen('CloseAll');

% Reenable Synctests after this simple demo:
Screen('Preference','SkipSyncTests',1);

% Well done!
return
