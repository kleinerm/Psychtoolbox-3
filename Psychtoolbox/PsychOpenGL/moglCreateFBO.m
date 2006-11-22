function [fbo , texids] = moglCreateFBO(width, height, nrbuffers, layers, format, withdepth, withstencil)
% [fbo , texids] = moglCreateFBO(width, height [, nrbuffers, layers, format, withdepth, withstencil])
%
% moglCreateFBO creates a standard OpenGL Framebuffer Object, suitable for
% computer vision and other GPGPU tasks and returns a handle to it.
%
% The FBO will have a size of width x height pixels and its 'nrbuffers'
% color buffers will have textures with 'layers' layers (default = 4 for RGBA)
% attached.
% 'format' specifies the data format. It defaults to single precision
% floating point resolution (32 bit float). If 'withdepth' > 0 then a
% depth buffer gets attached as well. If 'withstencil' > 0 then a stencil
% drawable gets attached as well.
%
% The function will create appropriate textures and renderbuffers, create
% an appropriate FBO and attach the textures and renderbuffers. After
% validation, the handle to the FBO is returned.

% History:
% 30.05.2006 Wrote it. (MK)

global GL;

% Child protection:
AssertGLSL;

% Hack, needs to be improved...
if IsLinux
    % Redefine vor NVidia:
    GL.RGBA_FLOAT32_APPLE = hex2dec('8883');
end;

if nargin < 2
    error('Must specify a widht x height of FBO in CreateGLFBO!');
end;

if nargin < 3
    nrbuffers = 1;
end;

if isempty(nrbuffers)
    nrbuffers = 1;
end;

if nargin < 4
    layers = 4;
end;
if isempty(layers)
    layers = 4;
end;

if nargin < 5
    format = GL.RGBA_FLOAT32_APPLE;
end;
if isempty(format)
    format = GL.RGBA_FLOAT32_APPLE;
end;

if nargin < 6
    withdepth = 0;
end;
if isempty(withdepth)
    withdepth = 0;
end;

if nargin < 7
    withstencil = 0;
end;
if isempty(withstencil)
    withstencil = 0;
end;

if nrbuffers > glGetIntegerv(GL.MAX_COLOR_ATTACHMENTS_EXT)
    error('moglCreateFBO: Sorry this hardware does not support the requested number of color buffers.');
end;

% Enable 2D rectangle textures. Power-of-two textures are known to make
% trouble on many older gfx-cards...
glEnable(GL.TEXTURE_RECTANGLE_EXT);

% Generate texture objects:
texids=glGenTextures(nrbuffers);

% Create a framebuffer object:
fbo = glGenFramebuffersEXT(1);

% Bind fbo:
glBindFramebufferEXT(GL.FRAMEBUFFER_EXT, fbo);

% Create and attach textures as color buffer attachments:
for i=1:nrbuffers
    % Enable texture by binding it:
    glBindTexture(GL.TEXTURE_RECTANGLE_EXT,texids(i));

    % Create representation: A rectangle texture with only mipmap level zero
    % and without a border, single precision float, RGBA:
    glTexImage2D(GL.TEXTURE_RECTANGLE_EXT, 0, format, width, height, 0, GL.RGBA, GL.FLOAT, 0);

    % Setup texture wrapping behaviour to clamp, as other behaviours are
    % unsupported on many gfx-cards for rectangle textures:
    glTexParameterfv(GL.TEXTURE_RECTANGLE_EXT,GL.TEXTURE_WRAP_S,GL.CLAMP);
    glTexParameterfv(GL.TEXTURE_RECTANGLE_EXT,GL.TEXTURE_WRAP_T,GL.CLAMP);

    % Setup filtering for the textures - Use nearest neighbour as floating
    % point filtering usually unsupported.
    glTexParameterfv(GL.TEXTURE_RECTANGLE_EXT,GL.TEXTURE_MAG_FILTER,GL.NEAREST);
    glTexParameterfv(GL.TEXTURE_RECTANGLE_EXT,GL.TEXTURE_MIN_FILTER,GL.NEAREST);

    % Choose texture application function to be a neutral REPLACE:
    glTexEnvfv(GL.TEXTURE_ENV,GL.TEXTURE_ENV_MODE,GL.REPLACE);

    % Unbind it after setup:
    glBindTexture(GL.TEXTURE_RECTANGLE_EXT, 0);

    % Attach textures mipmap level zero as color buffer:
    glFramebufferTexture2DEXT(GL.FRAMEBUFFER_EXT, GL.COLOR_ATTACHMENT0_EXT + i - 1, GL.TEXTURE_RECTANGLE_EXT, texids(i), 0);    
end;
    
% Check if FBO is framebuffer complete:
fbostatus = glCheckFramebufferStatusEXT(GL.FRAMEBUFFER_EXT);
if not(fbostatus == GL.FRAMEBUFFER_COMPLETE_EXT)
    glDeleteFramebuffersEXT(1, fbo);
    glDeleteTextures(length(texids), texids);
    fprintf('Error code from framebuffer status call: %i\n', fbostatus);
    error('Failed to setup framebuffer object!');
    return;
end;
    
% Unbind it:
glBindFramebufferEXT(GL.FRAMEBUFFER_EXT, 0);

% Done. Return handle to FBO and texture:
return;
