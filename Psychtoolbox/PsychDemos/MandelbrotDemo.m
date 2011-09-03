function MandelbrotDemo
% MandelbrotDemo -- GPU visualization of the fractal Mandelbrot set.
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
% program is read from a file, compiled and then attached to the texture on
% texture creation time. Procedural textures only work with graphics
% hardware that has sufficiently advanced support for at least hardware
% fragment shaders.
%
% This demo implements the mathematical formula for visualization of the
% famous fractal Mandelbrot set in a fragment shader. The shader gets
% attached to a purely virtual texture. The texture is drawn via the
% standard Screen('DrawTexture') command -- your graphics processor
% generates the image content of the Mandelbrot set on the fly during
% drawing of the texture via evaluation of the mandelbrot formula.
% Selecting a subregion (the sourceRect parameter of 'DrawTexture') allows
% to zoom into a specific subregion of the Mandelbrot set.
%
% The implementation uses up to 150 iterations of the equation for each
% drawn pixel, so it may be slow on old hardware.
%
% In the demo, use gentle mouse movements to scroll around in the set and
% the left- right- mouse button to zoom into/out of the set. Press any key
% on the keyboard to exit the demo.
%
% Wikipedia http://www.wikipedia.org has a nice explanation of the
% Mandelbrot set under the search term "Mandelbrot set".
%
% The shader used for this demo is part of the "Orange book" - The guide to
% the OpenGL Shading language. The code is copyright 3DLabs. See the
% 3DLabs-License.txt file in the PsychDemos/OpenGLDemos/GLSLDemoShaders/
% subfolder for the license.

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

% Query width and height of window:
[tw, th] = Screen('WindowSize', win);

% Load the shader program from source file 'MandelbrotShader.frag.txt',
% compile it and get a handle to the GLSL program:
MandelbrotShader = LoadGLSLProgramFromFiles([ PsychtoolboxRoot 'PsychDemos/MandelbrotShader'], 1);

% Create purely virtual procedural texture. The texture has a width x
% height x depth of 1 x 1 x 1 texels (the only interesting area in the
% mandelbrot complex number plane). Attach the GLGL MandelbrotShader
% program which defines the texture algorithmically:
mandelbrottex = Screen('SetOpenGLTexture', win, [], 0, GL.TEXTURE_RECTANGLE_EXT, 1, 1, 1, MandelbrotShader);

% Activate the shader and setup some fractal parameters:
glUseProgram(MandelbrotShader);
glUniform2f(glGetUniformLocation(MandelbrotShader, 'center'), -0.57, -0.01);
glUniform1f(glGetUniformLocation(MandelbrotShader, 'zoom'), 1);
% Shader set up, deactivate it:
glUseProgram(0);

% Set good start values for zoom factor and position:
zoom = 1;
srcRect = CenterRectOnPoint([-0.5 -0.5 0.5 0.5]/zoom, 0.5, 0.5);
Screen('DrawTexture', win, mandelbrottex, srcRect, winRect);

% Position the mouse cursor at the center of the screen:
SetMouse(tw/2, th/2, win);

% Show some usage info text:
Screen('TextSize', win, 24);
Screen('TextStyle', win, 1);
DrawFormattedText(win, 'MandelbrotDemo\n\nGently move the mouse cursor to navigate in the Mandelbrot-Plane\nPress Left-/Right- mouse button to zoom in/out\n\nPress any key to quit\n\nPress mouse button to start.', 'center', 'center', [255 255 0]);

% Initial flip to show help text and sync us to the retrace:
Screen('Flip', win);

% Wait for mouse click to start animation loop:
GetClicks(win);

% Sync us to retrace and flip:
vbl = Screen('Flip', win);
ts = vbl;
count = 0;

% Animation loop. Runs until any key is pressed:
while ~KbCheck
    % Increment framecounter:
    count = count + 1;
    
    % Query mouse position and state:
    [x y buttons] = GetMouse(win);

    % Left mouse button pressed?
    if buttons(1)
        % Increase zoom factor:
        zoom = zoom + 0.01 * zoom;
    end

    % Right mouse button pressed?
    if buttons(2)
        % Decrease zoom factor:
        zoom = zoom - 0.01 * zoom;
    end

    % Map the current mouse pointer position to the area of the texture
    % that should be drawn - the area of interest in the complex number
    % plane:
    xt = (x/tw) * RectWidth(srcRect) + srcRect(RectLeft);
    yt = (y/th) * RectHeight(srcRect) + srcRect(RectTop);
    srcRect = CenterRectOnPoint([-0.5 -0.5 0.5 0.5]/zoom, xt, yt);
    
    % Draw the selected subregion srcRect of the Mandelbrot texture and
    % zoom it up to cover the whole window area winRect:
    Screen('DrawTexture', win, mandelbrottex, srcRect, winRect);

    % Show updated image via buffer flip at next retrace:
    vbl = Screen('Flip', win, vbl+0.005);
end

% Done. Print some runtime stats:
avgfps = count / (GetSecs - ts)

% Close the window, release all ressources:
Screen('CloseAll');

% Done.
