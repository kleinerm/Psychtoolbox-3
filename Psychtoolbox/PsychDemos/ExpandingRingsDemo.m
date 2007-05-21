function ExpandingRingsDemo
% ExpandingRingsDemo -- Generate an "expanding rings" stimulus by use of GLSL
% shaders and Psychtoolbox procedural textures.
%
% This demo illustrates the use of "procedural textures" with Psychtoolbox.
% A procedural texture is a texture that is not directly represented by an image
% matrix in memory, but the image content of the texture is generated on
% the fly during drawing of the texture by means of a small algorithm - a
% shader. The shader implements some mathematical formula or model which is
% evaluated to generate image content, or it reads content of a data
% matrix and transforms it into a picture. Psychtoolbox supports both,
% purely virtual textures of unlimited size that are purely algorithmically
% generated, and hybrid textures where an algorithm transforms the textures
% content into something to be drawn.
%
% The algorithm has to be implemented by a GLSL shader program - a vertex shader,
% primitive shader, fragment shader or any combination of them. The shader
% program is read from a file, compiled and then attached to the texture at
% texture creation time. Procedural textures only work with graphics
% hardware that has sufficiently advanced support for (at least) hardware
% fragment shaders.
%
% This demo implements a procedural texture which shows a set of rings that
% can expand and move in a fragment shader. The shader gets
% attached to a purely virtual texture. The texture is drawn via the
% standard Screen('DrawTexture') command -- your graphics processor
% generates the image content of the stimulus on the fly during
% drawing of the texture via execution of the shader at each output pixel
% location.

% History:
% 19.05.2007 Written (MK)

% Acquire a handle to OpenGL, so we can use OpenGL commands in our code:
global GL;

% Make sure this is running on OpenGL Psychtoolbox:
AssertOpenGL;

% Choose screen with maximum id - the secondary display:
screenid = max(Screen('Screens'));

% Open a fullscreen onscreen window on that display, choose a background
% color of 128 = gray with 50% max intensity:
[win winRect]= Screen('OpenWindow', screenid, 128);

% Query window size: Need this to define center and radius of expanding
% disk stimulus:
[tw, th] = Screen('WindowSize', win);

% Load the 'ExpandingRingsShader' fragment program from file, compile it,
% return a handle to it:
expandingRingShader = LoadGLSLProgramFromFiles([PsychtoolboxRoot 'PsychDemos/ExpandingRingsShader'], 1);

% Create a purely virtual texture 'ringtex' of size tw x th virtual pixels, i.e., the
% full size of the window. Attach the expandingRingShader to it, to define
% its "appearance":
ringtex = Screen('SetOpenGLTexture', win, [], 0, GL.TEXTURE_RECTANGLE_EXT, tw, th, 1, expandingRingShader);

% Bind the shader: After binding it, we can setup some parameters for our
% stimulus:
glUseProgram(expandingRingShader);

% Set the 'RingCenter' parameter to the center position of the ring
% stimulus [tw/2, th/2]:
glUniform2f(glGetUniformLocation(expandingRingShader, 'RingCenter'), tw/2, th/2);

% Set the width of a single ring in pixels to 20 pixels:
glUniform1f(glGetUniformLocation(expandingRingShader, 'RingWidth'), 20);

% Set color of the odd rings to [1.0, 1.0, 0.0, 1.0] == yellow with full alpha: Note that
% color are spec'd in range 0.0 - 1.0, not in the "normal" Psychtoolbox
% color range of 0 - 255. This is because we use a standard OpenGL command
% for color setup -- OpenGL expects values in this range. Normally
% Psychtoolbox-3 hides this from us by internally remapping color values for
% backwards compatibility with the old PTB-2:
glUniform4f(glGetUniformLocation(expandingRingShader, 'secondColor'), 1.0, 1.0, 0.0, 1.0);

% Retrieve handles to the 'Shift' and 'Radius' parameters inside the
% shader: The shiftparam allows to shift the ring stimulus -- scrolling it.
% The radiusparam allows to change the radius of the outermost ring - the
% total expansion of the bulleye ring stimulus:
shiftparam  = glGetUniformLocation(expandingRingShader, 'Shift');
radiusparam = glGetUniformLocation(expandingRingShader, 'Radius');

% Done with setup, disable shader:
glUseProgram(0);

% Initial shift- and radius value is zero: We start with a point in the
% center of the screen which will then expand and scroll by one pixel at
% each redraw:
shiftvalue = 0;
count = 0;

% Perform initial flip to gray background and sync us to the retrace:
vbl = Screen('Flip', win);
ts = vbl;

% Animation loop: Run until keypress:
while ~KbCheck
    % Increase shift and radius of stimulus:
    shiftvalue = shiftvalue + 1;
    count = count + 1;
    
    % Assign the new shift- and radius values to the texture shader:
    glUseProgram(expandingRingShader);
    % Assign 'shiftvalue' as new setting for 'shiftparam':
    glUniform1f(shiftparam, shiftvalue);
    % Assign 'shiftvalue' as new setting for 'radiusparam':
    glUniform1f(radiusparam, shiftvalue);
    % Done with stimulus setup for this iteration:
    glUseProgram(0);
    
    % Draw the stimulus with its current parameter settings. We simply draw
    % the procedural texture as any other texture via 'DrawTexture'. We
    % leave all draw settings to their defaults, except the base drawing
    % color, which in our case defines the color of the even rings. A
    % setting of [red green blue] == [255 0 0] will set them to red color.
    % These colors are spec'd in the 0-255 range because they are input to
    % a standard PTB command.
    Screen('DrawTexture', win, ringtex, [], [], [], [], [], [255 0 0]);
    
    % Request stimulus onset at next video refresh:
    vbl = Screen('Flip', win);
end

% Done. Print some fps stats:
avgfps = count / (vbl - ts)

% Close window, release all ressources:
Screen('CloseAll');
