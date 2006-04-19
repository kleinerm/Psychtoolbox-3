function GLSLDemo
% GLSLDemo - Demonstrate use of the GLSL OpenGL Shading language in the
% Psychtoolbox.
%
% The OpenGL shading language (GLSL) allows to write specialized programs
% which are uploaded into the graphics hardware itself. These programs
% are then executed very efficiently in the grahpics hardware. So called
% Vertex-Shaders are executed on a per-vertex basis. They calculate and
% manipulate per-vertex properties, e.g., displacement, color, normal
% vectors. So called Fragment-Shaders are executed for each single fragment
% or pixel that is drawn into the backbuffer. They allow, e.g., to implement
% your own lighting model or perform image processing on your image.
%
% GLSL programs are very similar in their syntax to the C programming language,
% its basically C, extended by functions for matrix and vector math and for
% typical graphics purpose.
%
% A specific piece of graphics hardware may only support either vertex-shaders,
% or fragment-shaders or none at all, as these features are pretty new in the
% world of computer-graphics, so this demo may not run at all or at least some
% shaders will either fail or give unexpected results. If you want to make
% use of GLSL you'll need to equip your computer with up to date graphics
% hardware.
%
% This demo loads a collection of shaders. It applies the shaders to a
% collection of objects, demonstrating some visual effects. This is early
% beta code, so don't expect too much. It mostly demonstrates how to get
% started with shader-programming in Psychtoolbox.
%
% Press the 'n' key to toggle between different objects.
% Press the SPACE key to toggle between the different shaders.
% Stop the demo by pressing any other key.

% Online specs for GLSL can be found under:
% <http://www.opengl.org/documentation/glsl/>
%
% This:
% <http://www.lighthouse3d.com/opengl/glsl/index.php?intro>
% is a very nice introduction into GLSL.
%
% Apart from that, theres the standard book on GLSL by Addison Wesley:
% "OpenGL(R) Shading Language (2nd Edition)" this is also known as
% "The Orange book".
%

% Written by Mario Kleiner.

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

% Make sure we run on a GLSL capable system. Abort if not.
AssertGLSL;

% Setup the OpenGL rendering context of the onscreen window for use by
% OpenGL wrapper. After this command, all following OpenGL commands will
% draw into the onscreen window 'win':
Screen('BeginOpenGL', win);

% Get the aspect ratio of the screen:
ar=winRect(4)/winRect(3);

% Turn on OpenGL local lighting model: The lighting model supported by
% OpenGL is a local Phong model with Gouraud shading.
glEnable(GL_LIGHTING);
glEnable(GL_NORMALIZE);

% Enable the first local light source GL_LIGHT_0. Each OpenGL
% implementation is guaranteed to support at least 8 light sources. 
glEnable(GL_LIGHT0);
glEnable(GL_LIGHT1);

% Enable two-sided lighting - Back sides of polygons are lit as well.
glLightModelfv(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);

% Enable proper occlusion handling via depth tests:
glEnable(GL_DEPTH_TEST);

% Define the cubes light reflection properties by setting up reflection
% coefficients for ambient, diffuse and specular reflection:
glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT, [ 0.88 0.1 0.88 1 ]);
glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE, [ .22 .27 .9 1 ]);
glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR, [ 1 1 1 1 ]);
glMaterialfv(GL_FRONT_AND_BACK,GL_SHININESS, 2);

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
glLightfv(GL_LIGHT0,GL_POSITION,[ 20 200 20 0 ]);
% Emits white (1,1,1,1) diffuse light:
glLightfv(GL_LIGHT0,GL_DIFFUSE, [ 0.4 0.4 0.9 1 ]);
% Emits reddish (1,1,1,1) specular light:
glLightfv(GL_LIGHT0,GL_SPECULAR, [ 1 1 1 1 ]);
% There's also some blue, but weak (R,G,B) = (0.1, 0.1, 0.1)
% ambient light present:
glLightfv(GL_LIGHT0,GL_AMBIENT, [ .0 .0 .9 1 ]);

% Point lightsource at (1,2,3)...
glLightfv(GL_LIGHT1,GL_POSITION,[ 20 -200 20 0 ]);
% Emits white (1,1,1,1) diffuse light:
glLightfv(GL_LIGHT1,GL_DIFFUSE, [ 0.8 0.8 0.2 1 ]);
% Emits reddish (1,1,1,1) specular light:
glLightfv(GL_LIGHT1,GL_SPECULAR, [ 1 1 1 1 ]);
% There's also some blue, but weak (R,G,B) = (0.1, 0.1, 0.1)
% ambient light present:
glLightfv(GL_LIGHT1,GL_AMBIENT, [ .0 .0 .9 1 ]);

glEnable(GL_NORMALIZE);

% GLSL setup:
glGetError;

% Load all pairs of GLSL shaders from the directory of demo shaders and
% create GLSL programs for them: LoadGLSLProgramFromFiles is a convenience
% function. It loads single shaders or multiple shaders from text files,
% compiles and links them into a GLSL program, checks for errors and - if
% everything is fine - returns a handle that can be used to enable the
% GLSL program via glUseProgram():
shaderpath = [PsychtoolboxRoot '/PsychDemos/OpenGL4MatlabDemos/GLSLDemoShaders/'];
glsl(1)=LoadGLSLProgramFromFiles([shaderpath 'Flattenshader'],1);
glsl(2)=LoadGLSLProgramFromFiles([shaderpath 'Pointlightshader'],1);
glsl(3)=LoadGLSLProgramFromFiles([shaderpath 'Toonshader'],1);
glsl(4)=LoadGLSLProgramFromFiles([shaderpath 'Brickshader'],1);

gluErrorString

% Activate program:
glUseProgram(glsl(1));
programid = glsl(1);
programmax = length(glsl);
gluErrorString

% Initialize amount and direction of rotation
theta=0;
rotatev=[ 0 0 1 ];

objectid=0;

% Animation loop: Run until key press...
while (1)
    % Calculate rotation angle for next frame:
    theta=mod(theta+0.03,360);
    rotatev=rotatev+0.01*[ sin((pi/180)*theta) sin((pi/180)*2*theta) sin((pi/180)*theta/5) ];
    rotatev=rotatev/sqrt(sum(rotatev.^2));

    % Setup cubes rotation around axis:
    glPushMatrix;
    glRotated(theta,rotatev(1),rotatev(2),rotatev(3));

    % Clear out the backbuffer: This also cleans the depth-buffer for
    % proper occlusion handling:
    glClear;
    switch mod(objectid, 8)
        case 0
            glutSolidCube(1.0);
        case 1
            glutSolidTeapot(1.0);
        case 2
            glutSolidSphere(1, 50, 10);
        case 3
            glutSolidTorus( 0.7, 1, 100, 100 )
        case 4
            glutWireCube(1.0);
        case 5
            glutWireTeapot(1.0);
        case 6
            glutWireSphere(1, 50, 10);
        case 7
            glutWireTorus( 0.7, 1, 100, 100 )
    end;

    glPopMatrix;
    
    % Finish OpenGL rendering into PTB window and check for OpenGL errors.
    Screen('EndOpenGL', win);

    % Show rendered image at next vertical retrace:
    Screen('Flip', win);

    % Switch to OpenGL rendering again for drawing of next frame:
    Screen('BeginOpenGL', win);
    
    % Check for keyboard press and exit, if so:
    [keydown secs keycode]=KbCheck;
    if keydown
        if keycode(KbName('n'))
            % Toggle objectid:
            objectid=objectid+1;
        elseif keycode(KbName('space'))
            % Toggle/Switch use of GLSL shaders:
            programid = mod(programid + 1, programmax+1);

            if programid > 0
                % Enable shaders:
                glUseProgram(glsl(programid));
            else
                % Disable shaders:
                glUseProgram(0);
            end;            
        else
            break;
        end;
        % Debounce keys:
        while KbCheck; end;
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
