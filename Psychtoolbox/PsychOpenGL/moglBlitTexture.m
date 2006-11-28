function moglBlitTexture(texid, x, y, w, h, fastblit, clamping)
% moglBlitTexture(texid, x, y, w, h, fastblit, clamping)
%
% moglBlitTexture blits the OpenGL texture 'texid' at offset (x,y) into the
% current framebuffer. 'w x h' pixels are blitted. By default the offset is
% (0,0), the size is the full size of the texture.
%
% If you set the fastblit - flat to 1, then all other parameters are
% required, all error-checking and texture engine setup is skipped and only
% the minimal amount of code for initiating a blit is performed. This is
% useful in inner loops of GPGPU algorithms to squeeze out more speed!
%
% This does basically the same as Screen('DrawTexture') but without all the
% convenience stuff contained in 'DrawTexture' - and without the possible
% interference of PTB's internal drawing model. This is mostly useful for
% GPGPU and image processing applications where one needs very strict
% control of the way drawing is done. For everything else, use
% Screen('DrawTexture').
%
% Current limitation: Only rectangle textures are supported.

% History:
% 30.5.2006 Wrote it (MK).

global GL;

if nargin >= 6
    if isempty(fastblit)
        fastblit = 0;
    end;
else
    fastblit = 0;
end;

% Should we take the fast path?
if fastblit == 0
    % Child prot.
    AssertGLSL;

    if nargin < 1
        error('Missing texture id texid.');
    end;

    % Bind it:
    glDisable(GL.TEXTURE_2D);
    glEnable(GL.TEXTURE_RECTANGLE_EXT);
    glBindTexture(GL.TEXTURE_RECTANGLE_EXT, texid);

    % Query size:
    tw = glGetTexLevelParameteriv(GL.TEXTURE_RECTANGLE_EXT, 0, GL.TEXTURE_WIDTH);
    th = glGetTexLevelParameteriv(GL.TEXTURE_RECTANGLE_EXT, 0, GL.TEXTURE_HEIGHT);

    % Setup default size of srcRect:
    if nargin < 4
        w = tw;
    end;
    if isempty(w);
        w = tw;
    end;

    if nargin < 5
        h = th;
    end;
    if isempty(h);
        h = th;
    end;

    if w<0 || h<0 || w>tw || h>th
        error('Invalid w x h parameter. Negative or bigger than texture size.')
    end;

    if nargin < 2
        x = 0;
    end;
    if isempty(x)
        x = 0;
    end;

    if nargin < 3
        y = 0;
    end;
    if isempty(y)
        y = 0;
    end;

    if nargin>=7
        if ~isempty(clamping)
            glTexParameteri(GL.TEXTURE_RECTANGLE_EXT, GL.TEXTURE_WRAP_S, clamping);
            glTexParameteri(GL.TEXTURE_RECTANGLE_EXT, GL.TEXTURE_WRAP_T, clamping);
        end
    end
else
    % Fast path:
    glBindTexture(GL.TEXTURE_RECTANGLE_EXT, texid);    
end;

% Blit it:
glBegin(GL.QUADS)
glTexCoord2f(0, 0);
glVertex2f(x, y);
glTexCoord2f(0, h);
glVertex2f(x, y+h);
glTexCoord2f(w, h);
glVertex2f(x+w, y+h );
glTexCoord2f(w, 0);
glVertex2f(x+w, y);
glEnd;

if fastblit > 0
    return;
end;

% Unbind it:
glBindTexture(GL.TEXTURE_RECTANGLE_EXT, 0);
glDisable(GL.TEXTURE_RECTANGLE_EXT);

% Done.
return;
