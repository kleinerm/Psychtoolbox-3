function [ xy, fps, bitdepth ] = glmOpen( device, resolution, stereo, attribs )

% glmOpen  Open a screen in the mogl interface
% 
% usage:  [ xy, fps, bitdepth ] = glmOpen( device, resolution, stereo, attribs )

% 09-Dec-2005 -- created (RFM)

% ---protected---

% load constants into global variables
evalin('caller','glmGetConst');
global GL GLU AGL

if nargin<1,
    device=1;
end
if nargin<2,
    resolution=[];
end
if nargin<3 || isempty(stereo),
    stereo=0;
end
if nargin<4 || isempty(attribs),
    attribs=[ AGL.ALL_RENDERERS AGL.RGBA AGL.DOUBLEBUFFER ...
              AGL.FULLSCREEN AGL.ACCELERATED AGL.NO_RECOVERY ...
              AGL.NONE ];
end

% move the command window to the front, to catch keyboard input
commandwindow;

% stop update process
glmSetUpdate(0);

% check whether stereo is required
if stereo,
    attribs=[ AGL.STEREO attribs ];
end

% open screen
if ~isempty(resolution),
    moglcore('glmOpen',device,resolution(1),resolution(2),32,int32(attribs));
else
    moglcore('glmOpen',device,-1,-1,-1,int32(attribs));
end

% get screen information
[xy,fps,bitdepth]=glmGetScreenInfo;

% set swap interval
glmSetSwapInterval(1);

% null the clut
glmSetClut(repmat((0:255)',[ 1 3 ]));
glClearColor(0,0,0,1);

% set projection matrix
glMatrixMode(GL.PROJECTION);
glLoadIdentity;
gluOrtho2D(0,xy(1),0,xy(2));

% set rendering options
glDisable(GL.BLEND);
glDisable(GL.DITHER);

% set up buffers
if stereo,
    % enable stereo
    glEnable(GL.STEREO);
    glReadBuffer(GL.FRONT_RIGHT);
    for i=1:2,
        glDrawBuffer(GL.BACK_LEFT);
        glClear(mor(GL.COLOR_BUFFER_BIT,GL.DEPTH_BUFFER_BIT,GL.ACCUM_BUFFER_BIT,GL.STENCIL_BUFFER_BIT));
        glDrawBuffer(GL.BACK_RIGHT);
        glClear(mor(GL.COLOR_BUFFER_BIT,GL.DEPTH_BUFFER_BIT,GL.ACCUM_BUFFER_BIT,GL.STENCIL_BUFFER_BIT));
        glmSwapBuffers;
    end
else
    glReadBuffer(GL.FRONT);
    glDrawBuffer(GL.BACK);
    for i=1:2,
        glClear(mor(GL.COLOR_BUFFER_BIT,GL.DEPTH_BUFFER_BIT,GL.ACCUM_BUFFER_BIT,GL.STENCIL_BUFFER_BIT));
        glmSwapBuffers;
    end
end

% set the font (courier, bold, 20 pixels)
glmSetFont(22,1,round(xy(2)/25));

% hide mouse cursor
glmSetMouse([],0);

% open a figure to catch keypresses
glmGetChar('open');

return
