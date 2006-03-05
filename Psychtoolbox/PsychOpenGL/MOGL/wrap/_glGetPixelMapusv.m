function values = glGetPixelMapusv( map )

% glGetPixelMapusv  Interface to OpenGL function glGetPixelMapusv
%
% usage:  values = glGetPixelMapusv( map )
%
% C function:  void glGetPixelMapusv(GLenum map, GLushort* values)

% 05-Mar-2006 -- created (generated automatically from header files)

% ---allocate---

if nargin~=1,
    error('invalid number of arguments');
end

values = uint16(0);

moglcore( 'glGetPixelMapusv', map, values );

return
