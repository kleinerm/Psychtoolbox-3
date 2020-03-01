function [id, rect, shader] = CreateProceduralColorGrating(windowPtr, width, height, ...
	color1, color2, radius, normalise)
% [id, rect, shader] = CreateProceduralColorGrating(windowPtr, width, height [, color1=[1 0 0]]  [, color2=[0 1 0]] [, radius=0])
%
% A procedural color grating shader that can generate either sinusoidal or square gratings varying between two colors. Initial parameters are color1, color2 and radius. When drawing the shader using Screen('DrawTexture|s'), you can pass additional values:
% * baseColor is the base color from which color1 & color2 are mixed.
% * contrast is the mixing amount from baseColor to color1|2
% * sigma is a smoothing value, when sigma == -1 a sinusoidal grating is produced. when sigma >= 0 a square wave grating is produced using sigma value of smoothing at the edge. Smoothing for squarewave grating edges can reduce jumping artifacts when squarewave gratings are drifted. Smoothing uses GLSL smoothstep function (hermite interpolation).
% ====INPUT VALUES
% width, height = "virtual size" of the GLSL shader.
% color1 & color2 = two colors for the grating peaks to vary between. When running (see ColorGratingDemo), you can pass baseColor parameter, from which color1 and color2 will be modulated via the contrast parameter. When contrast == 0, then Color1 and color2 are the same as baseColor. As contrast increases each color is mixed with baseColor until at contrast == 1 the grating peaks are color1 and color2 exactly.Color mixing uses OpenGL's mix command. 
% radius = optional hard-edged circular mask sized in pixels
% normalise = do we use pow() to correct for a 2.2 gamma?
%
% ====RETURN VALUES
% id = texture pointer
% rect = default position rect
% shader = the GLSL shader, this is useful if you want to use glUniform commands to modify color 1, color2 or radius after texture creation.
%
% History:
% 06/06/2014 Created by Ian Andolina <http://github.com/iandol>, licenced under the MIT Licence

global GL;

AssertGLSL;

if nargin < 1 || isempty(windowPtr) 
	error('You must provide a PTB window pointer!');
end

if nargin < 2 || isempty(width) 
	width = 500;
end

if nargin < 3 || isempty(height) 
	height = width;
end

if nargin < 4 || isempty(color1)
	color1 = [1 0 0 1];
elseif length(color1) == 3
	color1 = [color1 1]; %add alpha
elseif length(color1) == 4
	% we are ok
else
	warning('color1 must be a 4 component RGBA vector [red green blue alpha], resetting color1 to red!');
	color1 = [1 0 0 1];
end

if nargin < 5 || isempty(color2)
	color2 = [0 1 0 1];
elseif length(color2) == 3
	color2 = [color2 1]; %add alpha
elseif length(color2) == 4
	% we are ok
else
	warning('color2 must be a 4 component RGBA vector [red green blue alpha], resetting color2 to green!');
	color2 = [0 1 0 1];
end

if nargin < 6 || isempty(radius)
	radius = 0;
end

if nargin < 7 || isempty(normalise)
	normalise = 0;
end

% Switch to windowPtr OpenGL context:
Screen('GetWindowInfo', windowPtr);

% Load shader:
p = mfilename('fullpath');
p = [fileparts(p) filesep];
shader = LoadGLSLProgramFromFiles({[p 'colorgrating.vert'], [p 'colorgrating.frag']}, 1);

% Setup shader:
glUseProgram(shader);
glUniform2f(glGetUniformLocation(shader, 'center'), width/2, height/2);
glUniform4f(glGetUniformLocation(shader, 'color1'), color1(1),color1(2),color1(3),color1(4));
glUniform4f(glGetUniformLocation(shader, 'color2'), color2(1),color2(2),color2(3),color2(4));
if radius>0
	glUniform1f(glGetUniformLocation(shader, 'radius'), radius); 
end
glUniform1f(glGetUniformLocation(shader, 'normalise'), normalise);
glUseProgram(0);

% Create a purely virtual procedural texture 'id' of size width x height virtual pixels.
% Attach the shader to it to define its appearance:
id = Screen('SetOpenGLTexture', windowPtr, [], 0, GL.TEXTURE_RECTANGLE_EXT, width, height, 1, shader);

% Query and return its bounding rectangle:
rect = Screen('Rect', id);