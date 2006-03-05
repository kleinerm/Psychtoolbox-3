function glmPutPixels( pixels )

% glmPutPixels  Draw packed pixels to the screen
%
% usage:  glmPutPixels( pixels )

% 01-Jan-2006 -- created (RFM)

% ---protected---

if nargin~=1,
    error('invalid number of arguments');
end

% decode parameters from pixel matrix
n=numel(pixels);
tag=double(pixels(n-7:end));
format=256*tag(1)+tag(2);
type=  256*tag(3)+tag(4);
width= 256*tag(5)+tag(6);
height=256*tag(7)+tag(8);

% call glDrawPixels
moglcore( 'glDrawPixels', width, height, format, type, pixels );

return
