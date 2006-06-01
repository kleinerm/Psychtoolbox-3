function moglChooseFBO(fbo, bufferid)
% moglChooseFBO(fbo, bufferid) -- Set FBO 'fbo' as framebuffer.
%
% If fbo is > 0, then the corresponding FBO is bound and OpenGL is set up
% to render and read from that FBO with orthogonal projection. If fbo is
% zero, then the system framebuffer is enabled again for normal drawing.
%
% Normally, read- and writebuffer are set to color attachment zero. If you
% provide bufferid, then it is set to bufferid, with 1 being the first
% attachment (COLOR_ATTACHMENT0_EXT).
%
% This function is mostly useful for image processing and GPGPU
% applications. Normal Psychtoolbox code will want to use standard
% Offscreen windows instead. They are properly managed by Psychtoolbox for
% normal stimulus drawing and implemented to work on any kind of hardware,
% whereas this function is optimized for OpenGL-2 compliant hardware.

% History:
% 30.05.2006 Wrote it (MK).

global GL;

% Child protection:
AssertGLSL;

if nargin < 1
    error('moglChooseFBO called without fbo argument.');
end;

if nargin < 2
    bufferid = 1;
end;

if isempty(bufferid)
    bufferid = 1;
end;

if fbo == 0
    % Unbind current FBO, reset to normal framebuffer:
    glBindFramebufferEXT(GL.FRAMEBUFFER_EXT, 0);
    % Restore read- draw-targets:
    glReadBuffer(GL.BACK);
    glDrawBuffer(GL.BACK);

    % TODO: Restore projection and viewport...
    return;
end;

% Child protection:
if ~glIsFramebufferEXT(fbo)
    error('Invalid fbo identifier passed. This is not a valid FBO.');
end;

if glGetIntegerv(GL.FRAMEBUFFER_BINDING_EXT)==0
    % Standard framebuffer currently bound. Query its
    % current viewport and matrices, so we can restore it.
    % TODO...
end;

% Bind new FBO:
glBindFramebufferEXT(GL.FRAMEBUFFER_EXT, fbo);

% Query its dimensions:
texid = glGetFramebufferAttachmentParameterivEXT(GL.FRAMEBUFFER_EXT, GL.COLOR_ATTACHMENT0_EXT, GL.FRAMEBUFFER_ATTACHMENT_OBJECT_NAME_EXT);
glBindTexture(GL.TEXTURE_RECTANGLE_EXT, texid);
fw = glGetTexLevelParameteriv(GL.TEXTURE_RECTANGLE_EXT, 0, GL.TEXTURE_WIDTH);
fh = glGetTexLevelParameteriv(GL.TEXTURE_RECTANGLE_EXT, 0, GL.TEXTURE_HEIGHT);
glBindTexture(GL.TEXTURE_RECTANGLE_EXT, 0);

% Setup drawing transforms, projection and viewport:
glMatrixMode(GL.PROJECTION);
glLoadIdentity;
gluOrtho2D(0, fw, 0 , fh);

glMatrixMode(GL.MODELVIEW);
glLoadIdentity;

glViewport(0,0,fw,fh);

% Setup read- and drawbuffers:
glReadBuffer(GL.COLOR_ATTACHMENT0_EXT + bufferid - 1);
glDrawBuffer(GL.COLOR_ATTACHMENT0_EXT + bufferid - 1);

% Ready to use:
return;
