function handle = LoadShaderFromFile(filename, shadertype, debug)
% handle = LoadShaderFromFile(filename [, shadertype])
% Loads a GLSL OpenGL shader definition from textfile 'filename' and
% creates an OpenGL GLSL shader of type 'shadertype'. Returns a handle to
% the new shader. If shadertype is omitted, the type of the shader is
% derived from the filename extension: .vert == GL_VERTEX_SHADER, .frag ==
% GL_FRAGMENT_SHADER. The optional 'debug' flag allows to enable debugging
% output.
%
% On successfull load & creation, a 'handle' to the new shader is returned.
%

% 29-Mar-2006 written by MK

global GL;

if isempty(GL)
    ptbmoglinit;
end;

% Make sure we run on a GLSL capable system.
AssertGLSL;

if nargin < 3
    debug = 0;
end;

if nargin < 1 | isempty(filename)
    % Set default shader file name:
    filename=[ fileparts(mfilename('fullpath')) '/Defaultshader.frag' ];
end;

if nargin < 2 | isempty(shadertype)
    % Try to autodetect shadertype from file extension:
    if ~isempty(strfind(filename, '.frag'))
        shadertype = GL.FRAGMENT_SHADER;
        if debug>0, fprintf('Building a fragment shader...\n'); end;
    elseif ~isempty(strfind(filename, '.vert'))
        shadertype = GL.VERTEX_SHADER;
        if debug>0, fprintf('Building a vertex shader...\n'); end;
    else
        error('No shadertype provided and could not derive type from filename extension!');
    end;
end;

if debug>0
    fprintf('Reading shader from file %s ...\n', filename);
end;

% Read shader source code from file:
fid=fopen(filename, 'r');
if fid<0
    error('Could not open shader definition file!');
end;
shadersrc = fread(fid);
fclose(fid);

% Create shader, assign sourcecode and compile it:
handle = glCreateShader(shadertype);
if debug > 1
    glShaderSource(handle, shadersrc, debug);
else
    glShaderSource(handle, shadersrc);
end;

glCompileShader(handle);

% Done.
return;
