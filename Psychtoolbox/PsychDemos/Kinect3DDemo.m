function Kinect3DDemo(stereomode)
% Kinect3DDemo - Capture and display video and depths data from a Kinect box.
%
% Usage:
%
% Kinect3DDemo(textureon, dotson, normalson, stereomode, usefastoffscreenwindows)
%
% This connects to a Microsoft Kinect device on the USB bus, then captures
% and displays video and depths data delivered by the Kinect.
%
% This is an early prototype.
%
% Control keys and their meaning:
% 'a' == Zoom out by moving object away from viewer.
% 'z' == Zoom in by moving object close to viewer.
% 'k' and 'l' == Rotate object around axis.
% 'q' == Quit demo.
%
% Options:
%
% textureon = If set to 1, the objects will be textured, otherwise they will be
% just shaded without a texture. Defaults to zero.
%
% dotson = If set to 0 (default), just show surface. If set to 1, some dots are
% plotted to visualize the vertices of the underlying mesh. If set to 2, the
% mesh itself is superimposed onto the shape. If set to 3 or 4, then the projected
% vertex 2D coordinates are also visualized in a standard Matlab figure window.
%
% normalson = If set to 1, then the surface normal vectors will get visualized as
% small green lines on the surface.
%
% stereomode = n. For n>0 this activates stereoscopic rendering - The shape is
% rendered from two slightly different viewpoints and one of Psychtoolbox's
% built-in stereo display algorithms is used to present the 3D stimulus. This
% is very preliminary so it doesn't work that well yet.
%
% usefastoffscreenwindows = If set to 0 (default), work on any graphics
% card. If you have recent hardware, set it to 1. That will enable support
% for fast offscreen windows - and a much faster implementation of shape
% morphing.

% History:
% 25.11.2010  mk  Written.

%
% This demo and the morpheable OBJ shapes were contributed by
% Dr. Quoc C. Vuong, MPI for Biological Cybernetics, Tuebingen, Germany.

morphnormals = 1;
global win;

% Is the script running in OpenGL Psychtoolbox?
AssertOpenGL;

%PsychDebugWindowConfiguration;
dst1 = [0, 0, 640, 480];
dst2 = [650, 0, 650+640, 480];

if nargin < 1 || isempty(stereomode)
    stereomode = 0;
end;
stereomode %#ok<NOPRT>

if nargin < 5 || isempty(usefastoffscreenwindows)
    usefastoffscreenwindows = 0;
end
usefastoffscreenwindows %#ok<NOPRT>

% Response keys: Mapping of keycodes to keynames.
KbName('UnifyKeyNames');
closer = KbName('a');
farther = KbName('z');
quitkey = KbName('ESCAPE');
rotateleft = KbName('LeftArrow');
rotateright = KbName('RightArrow');
toggleCapture = KbName('space');

% Find the screen to use for display:
screenid=max(Screen('Screens'));

% Disable Synctests for this simple demo:
Screen('Preference','SkipSyncTests',1);

% Setup Psychtoolbox for OpenGL 3D rendering support and initialize the
% mogl OpenGL for Matlab wrapper. We need to do this before the first call
% to any OpenGL function:
%InitializeMatlabOpenGL(0,1);
InitializeMatlabOpenGL(0,0);

% Open a double-buffered full-screen window: Everything is left at default
% settings, except stereomode:
rect = [];
%rect = [0 0 1300 500];

if stereomode > 0
    PsychImaging('PrepareConfiguration');
    [win , winRect] = PsychImaging('OpenWindow', screenid, 0, rect, [], [], stereomode);
else
    [win , winRect] = Screen('OpenWindow', screenid, 0, rect, [], [], stereomode);
end

if stereomode > 5 && stereomode < 10
	SetAnaglyphStereoParameters('OptimizedColorAnaglyphMode', win);
end

% Setup the OpenGL rendering context of the onscreen window for use by
% OpenGL wrapper. After this command, all following OpenGL commands will
% draw into the onscreen window 'win':
Screen('BeginOpenGL', win);

% Get the aspect ratio of the screen, we need to correct for non-square
% pixels if we want undistorted displays of 3D objects:
ar=winRect(4)/winRect(3);

% Enable proper occlusion handling via depth tests:
glEnable(GL.DEPTH_TEST);

% Set projection matrix: This defines a perspective projection,
% corresponding to the model of a pin-hole camera - which is a good
% approximation of the human eye and of standard real world cameras --
% well, the best aproximation one can do with 3 lines of code ;-)
glMatrixMode(GL.PROJECTION);
glLoadIdentity;

% Field of view is +/- 25 degrees from line of sight. Objects close than
% 0.1 distance units or farther away than 200 distance units get clipped
% away, aspect ratio is adapted to the monitors aspect ratio:
gluPerspective(25.0,1/ar,0.1,10000.0);

% Setup modelview matrix: This defines the position, orientation and
% looking direction of the virtual camera:
glMatrixMode(GL.MODELVIEW);
glLoadIdentity;

% Set size of points for drawing of reference dots
glPointSize(3.0);

% Setup initial z-distance of objects:
zz = 50.0;
ang = 0.0;      % Initial rotation angle

% Half eye separation in length units for quick & dirty stereoscopic
% rendering. Our way of stereo is not correct, but it makes for a
% nice demo. Figuring out proper values is not too difficult, but
% left as an exercise to the reader.
eye_halfdist=3;
if stereomode == 0
	eye_halfdist=0;
end

% Finish OpenGL setup and check for OpenGL errors:
Screen('EndOpenGL', win);

% Retrieve duration of a single monitor flip interval: Needed for smooth
% animation.
ifi = Screen('GetFlipInterval', win);

kinect = PsychKinect('Open');
PsychKinect('SetAngle', kinect, -30);
WaitSecs('YieldSecs', 0.5);
for angle = -20:20
	PsychKinect('SetAngle', kinect, angle);
	WaitSecs('YieldSecs', 0.050);
end
PsychKinect('SetAngle', kinect, 0);

PsychKinect('Start', kinect);

% Initially sync us to the VBL:
vbl=Screen('Flip', win);

% Some stats...
tstart=vbl;
framecount = 0;
waitframes = 1;

doCapture = 1;
oldKeyIsDown = KbCheck;
tex1 = [];
tex2 = [];
mymesh = [];

% Animation loop: Run until key press or one minute has elapsed...
t = GetSecs;
while ((GetSecs - t) < 600)
    if doCapture
	if 1 && ~isempty(mymesh)
	    PsychKinect('DeleteObject', win, mymesh);
	    mymesh = [];
	end

    	[rc, cts] = PsychKinect('GrabFrame', kinect, 1);
	    if rc > 0

		if 0
	        	[imbuff, width, height, channels] = PsychKinect('GetImage', kinect, 1, 1);
			if width > 0 && height > 0
				tex2 = Screen('SetOpenGLTextureFromMemPointer', win, tex2, imbuff, width, height, channels, 1, GL.TEXTURE_RECTANGLE_EXT);
			end
		else
			if ~isempty(tex2)
				Screen('Close', tex2);
			end
			tex2 = [];
		end

		mymesh = PsychKinect('CreateObject', win, kinect);
	        PsychKinect('ReleaseFrame', kinect);

		if isempty(mymesh)
			continue;
		end
	    else
		continue;
	    end
    end

    if 0 && ~isempty(mymesh.tex) && (mymesh.tex > 0)
	Screen('DrawTexture', win, mymesh.tex, [], dst1);
    end

    if ~isempty(tex2) && (tex2 > 0)
	Screen('DrawTexture', win, tex2, [], dst2);
    end

    % Switch to OpenGL rendering for drawing of next frame:
    Screen('BeginOpenGL', win);

    % Left-eye cam is located at 3D position (-eye_halfdist,0,zz), points upright (0,1,0) and fixates
    % at the origin (0,0,0) of the worlds coordinate system:
    glLoadIdentity;
    gluLookAt(-eye_halfdist, 0, zz, 0, 0, 0, 0, 1, 0);

    % Draw into image buffer for left eye:
    Screen('EndOpenGL', win);
    Screen('SelectStereoDrawBuffer', win, 0);
    Screen('BeginOpenGL', win);

    % Clear out the depth-buffer for proper occlusion handling:
    glClear(GL.DEPTH_BUFFER_BIT);

    % Call our subfunction that does the actual drawing of the shape (see below):
    drawShape(mymesh, ang);
    
    % Stereo rendering requested?
    if (stereomode > 0)
        % Yes! We need to render the same object again, just with a different
        % camera position, this time for the right eye:
                
        % Right-eye cam is located at 3D position (+eye_halfdist,0,zz), points upright (0,1,0) and fixates
        % at the origin (0,0,0) of the worlds coordinate system:
        glLoadIdentity;
        gluLookAt(+eye_halfdist, 0, zz, 0, 0, 0, 0, 1, 0);
        % Draw into image buffer for right eye:
        Screen('EndOpenGL', win);
        Screen('SelectStereoDrawBuffer', win, 1);
        Screen('BeginOpenGL', win);
    
        % Clear out the depth-buffer for proper occlusion handling:
        glClear(GL.DEPTH_BUFFER_BIT);
        
        % Call subfunction that does the actual drawing of the shape (see below):
        drawShape(mymesh, ang);
    end;
    
    % Finish OpenGL rendering into Psychtoolbox - window and check for OpenGL errors.
    Screen('EndOpenGL', win);

    % Tell Psychtoolbox that drawing of this stim is finished, so it can optimize
    % drawing:
    Screen('DrawingFinished', win);

    % Now that all drawing commands are submitted, we can do the other stuff before
    % the Flip:
    
    % Check for keyboard press:
    [KeyIsDown, endrt, KeyCode] = KbCheck;
    if KeyIsDown
        if ( KeyIsDown==1 & KeyCode(closer)==1 ) %#ok<AND2>
            zz=zz-1;
            KeyIsDown=0;
        end

        if ( KeyIsDown==1 & KeyCode(farther)==1 ) %#ok<AND2>
            zz=zz+1;
            KeyIsDown=0;
        end

        if ( KeyIsDown==1 & KeyCode(rotateright)==1 ) %#ok<AND2>
            ang=ang+1.0;
            KeyIsDown=0;
        end

        if ( KeyIsDown==1 & KeyCode(rotateleft)==1 ) %#ok<AND2>
            ang=ang-1.0;
            KeyIsDown=0;
        end
    end

    if KeyIsDown && ~oldKeyIsDown
	oldKeyIsDown = KeyIsDown;

        if ( KeyIsDown==1 & KeyCode(toggleCapture)==1 ) %#ok<AND2>
            doCapture = ~doCapture
            KeyIsDown=0;
        end

        if ( KeyIsDown==1 & KeyCode(quitkey)==1 ) %#ok<AND2>
            break;
        end
    else
	oldKeyIsDown = KeyIsDown;
    end


    % Update frame animation counter:
    framecount = framecount + 1;

    % We're done for this frame:

    % Show rendered image 'waitframes' refreshes after the last time
    % the display was updated and in sync with vertical retrace:
    vbl = Screen('Flip', win, vbl + (waitframes - 0.5) * ifi);
    %Screen('Flip', win, 0, 0, 2);
end

vbl = Screen('Flip', win);

% Calculate and display average framerate:
fps = framecount / (vbl - tstart) %#ok<NOPRT,NASGU>

if ~isempty(mymesh)
    PsychKinect('DeleteObject', win, mymesh);
    mymesh = [];
end

PsychKinect('Stop', kinect);
PsychKinect('Close', kinect);

% Close onscreen window and release all other ressources:
%Screen('Flip', win);
Screen('CloseAll');

% Reenable Synctests after this simple demo:
Screen('Preference','SkipSyncTests',1);

% Well done!
return

% drawShape does the actual drawing:
function drawShape(mymesh, ang)
% GL needs to be defined as "global" in each subfunction that
% executes OpenGL commands:
global GL
global win

% Backup modelview matrix:
glPushMatrix;

% Setup rotation around axis:
glTranslatef(0,0,-10);
glRotated(ang,0,1,0);
glTranslatef(0,0,+10);
glRotated(180,0,1,0);
glRotated(180,0,0,1);

% Scale object by a factor of a:
a=10;
glScalef(a,a,a);

% Render!
PsychKinect('RenderObject', win, mymesh);

% Restore modelview matrix:
glPopMatrix;

% Done, return to main:
return;

