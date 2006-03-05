function glPixelMapusv( map, mapsize, values )

% glPixelMapusv  Interface to OpenGL function glPixelMapusv
%
% usage:  glPixelMapusv( map, mapsize, values )
%
% C function:  void glPixelMapusv(GLenum map, GLint mapsize, const GLushort* values)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glPixelMapusv', map, mapsize, uint16(values) );

return
