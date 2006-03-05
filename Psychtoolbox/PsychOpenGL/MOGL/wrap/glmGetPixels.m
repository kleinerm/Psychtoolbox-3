function pixels = glmGetPixels( rect )

% glmGetPixels  Read packed pixels from the screen
%
% usage:  pixels = glmGetPixels( rect )

% 01-Jan-2006 -- created (RFM)

% ---allocate---
% ---protected---

if nargin~=1,
    error('invalid number of arguments');
end

% set parameters
global GL
format=GL.RGB;
type=GL.UNSIGNED_BYTE;
width=rect(3)-rect(1);
height=rect(4)-rect)2);

% allocate return argument
padn=mod( 4-mod(3*width,4) , 4 );
n=(width+padn)*height+8;
pixels=uint8(zeros(n,1));

% call glReadPixels
moglcore( 'glReadPixels', rect(1), rect(2), width, height, ...
    format, type, pixels );

% encode format, type, width, and height at end of matrix
pixels(end-7:end)=[ floor(format/256) mod(format,256) ...
                    floor(type/256)   mod(type,256)   ...
                    floor(width/256)  mod(width,256)  ...
                    floor(height/256) mod(height,256) ];

return
