function values = glGetPixelMapfv( map )

% glGetPixelMapfv  Interface to OpenGL function glGetPixelMapfv
%
% usage:  values = glGetPixelMapfv( map )
%
% C function:  void glGetPixelMapfv(GLenum map, GLfloat* values)

% 24-Jan-2006 -- created (generated automatically from header files)

% ---allocate---
% ---protected---
% ---skip---

if nargin~=1,
    error('invalid number of arguments');
end

% find map size
global GL
maxn=glGetIntegerv(GL.MAX_PIXEL_MAP_TABLE);
values = single(NaN(maxn,1));
moglcore( 'glGetPixelMapfv', map, values );
values = double(values);
values = values(~isnan(values));

return
