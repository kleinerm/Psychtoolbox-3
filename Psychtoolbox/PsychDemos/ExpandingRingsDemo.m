function ExpandingRingsDemo(ringtype)
% ExpandingRingsDemo([ringtype=0]) -- Generate an "expanding rings"
% stimulus by use of GLSL shaders and Psychtoolbox procedural textures.
%
% This demo illustrates the use of "procedural textures" with Psychtoolbox.
% A procedural texture is a texture that is not directly represented by an
% image matrix in memory, but the image content of the texture is generated
% on the fly during drawing of the texture by means of a small algorithm -
% a shader. The shader implements some mathematical formula or model which
% is evaluated to generate image content, or it reads content of a data
% matrix and transforms it into a picture. Psychtoolbox supports both,
% purely virtual textures of unlimited size that are purely algorithmically
% generated, and hybrid textures where an algorithm transforms the textures
% content into something to be drawn.
%
% The algorithm has to be implemented by a GLSL shader program - a vertex
% shader, geometry shader, fragment shader or any combination of them. The
% shader program is read from a file, compiled and then attached to the
% texture at texture creation time. Procedural textures only work with
% graphics hardware that has sufficiently advanced support for (at least)
% hardware fragment shaders.
%
% This demo implements a procedural texture which shows a set of rings that
% can expand and move. The shader gets attached to a purely virtual
% texture. The texture is drawn via the standard Screen('DrawTexture')
% command -- your graphics processor generates the image content of the
% stimulus on the fly during drawing of the texture via execution of the
% shader at each output pixel location.
%
% The optional 'ringtype' parameter allows to select between different ring
% shapes. Default type is zero:
%
% 0 = Hard transitions between red and yellow rings.
% 1 = Transitions are modeled as a smooth sine wave, softly fading from
%     yellow to red and back.
%

% History:
% 19.05.2007 Written. (MK)
% 24.04.2011 Updated to support 'ringtype' parameter and sine-wave rings. (MK)
% 12.06.2011 Updated to use ProceduralShadingAPI for cleaner M-file code. (MK)


% Acquire a handle to OpenGL, so we can use OpenGL commands in our code:
global GL;

% Make sure this is running on OpenGL Psychtoolbox:
AssertOpenGL;

if nargin < 1 || isempty(ringtype)
    ringtype = 0;
end

% Choose screen with maximum id - the secondary display:
screenid = max(Screen('Screens'));

% Open a fullscreen onscreen window on that display, choose a background
% color of 128 = gray with 50% max intensity:
win = Screen('OpenWindow', screenid, 128);

% Switch color specification to use the 0.0 - 1.0 range instead of the 0 -
% 255 range. This is more natural for these kind of stimuli:
Screen('ColorRange', win, 1);

% Query window size: Need this to define center and radius of expanding
% disk stimulus:
[tw, th] = Screen('WindowSize', win);

% Load the 'ExpandingRingsShader' fragment program from file, compile it,
% return a handle to it:
rshader = [PsychtoolboxRoot 'PsychDemos/ExpandingRingsShader.vert.txt'];

if ringtype == 0
    expandingRingShader = LoadGLSLProgramFromFiles({ rshader, [PsychtoolboxRoot 'PsychDemos/ExpandingRingsShader.frag.txt'] }, 1);
    % Width of a single ring (radius) in pixels:
    ringwidth = 20;
else
    expandingRingShader = LoadGLSLProgramFromFiles({ rshader, [PsychtoolboxRoot 'PsychDemos/ExpandingSinesShader.frag.txt'] }, 1);
    % Width of a single ring (radius) / Period of a single color sine wave in pixels:
    ringwidth = 200;
end

% Create a purely virtual texture 'ringtex' of size tw x th virtual pixels, i.e., the
% full size of the window. Attach the expandingRingShader to it, to define
% its "appearance":
ringtex = Screen('SetOpenGLTexture', win, [], 0, GL.TEXTURE_RECTANGLE_EXT, tw, th, 1, expandingRingShader);

% Bind the shader: After binding it, we can setup some constant parameters
% for our stimulus, so called GLSL 'uniform' variables. These are
% parameters that are constant during the whole session, or at least only
% change infrequently. They are set outside the fast stimulus rendering
% loop and potentially optimized by the graphics driver for fast execution:
glUseProgram(expandingRingShader);

% Set the 'RingCenter' parameter to the center position of the ring
% stimulus [tw/2, th/2]:
glUniform2f(glGetUniformLocation(expandingRingShader, 'RingCenter'), tw/2, th/2);

% Done with setup, disable shader. All other stimulus parameters will be
% set at each Screen('DrawTexture') invocation to allow fast dynamic change
% of the parameters during each stimulus redraw:
glUseProgram(0);

% Define first and second ring color as RGBA vector with normalized color
% component range between 0.0 and 1.0:
firstColor  = [1 0 0 1]; 
secondColor = [1 1 0 1]; 

% Initial shift- and radius value is zero: We start with a point in the
% center of the screen which will then expand and scroll by one pixel at
% each redraw until it fills the whole screen, continuing to scroll
% periodically:
shiftvalue = 0;
count = 0;

% Retrieve monitor refresh duration:
ifi = Screen('GetFlipInterval', win);

% Perform initial flip to gray background and sync us to the retrace:
vbl = Screen('Flip', win);
ts = vbl;

% Animation loop: Run until keypress:
while ~KbCheck
    % Increase shift and radius of stimulus:
    shiftvalue = shiftvalue + 1;
    radius = shiftvalue;
    count = count + 1;
    
    % Draw the stimulus with its current parameter settings. We simply draw
    % the procedural texture as any other texture via 'DrawTexture'. We
    % leave all draw settings to their defaults, except the base drawing
    % colors, which in our case defines the colors of the rings and the
    % ringWidth, Radius and Shift parameters:
    % The vector with additional parameters must have a length which is a
    % multiple of four. Therefore - as we only have 7 parameters here - we
    % pad the vector with an additional meaningless zero value to end up
    % with a total length of 8 vector components.
    %
    % We use 'firstColor' for the even rings, 'secondColor' for the odd
    % rings.
    Screen('DrawTexture', win, ringtex, [], [], [], [], [], firstColor, [], [], [secondColor(1), secondColor(2), secondColor(3), secondColor(4), shiftvalue, ringwidth, radius, 0]);
    
    % Request stimulus onset at next video refresh:
    vbl = Screen('Flip', win, vbl + ifi/2);
end

% Done. Print some fps stats:
avgfps = count / (vbl - ts);
fprintf('Average redraw rate in Hz was: %f\n', avgfps);

% Close window, release all ressources:
Screen('CloseAll');
