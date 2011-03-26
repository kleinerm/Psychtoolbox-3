function values = glGetPixelMapusv( map )

% glGetPixelMapusv  Interface to OpenGL function glGetPixelMapusv
%
% usage:  values = glGetPixelMapusv( map )
%
% C function:  void glGetPixelMapusv(GLenum map, GLushort* values)

% 24-Jan-2006 -- created (generated automatically from header files)

% ---allocate---
% ---protected---

if nargin~=1,
    error('invalid number of arguments');
end

% a hack to find out how many values are returned
f=glGetPixelMapfv(map);

values = uint16(zeros(size(f)));
moglcore( 'glGetPixelMapusv', map, values );

return
% ---skip---
