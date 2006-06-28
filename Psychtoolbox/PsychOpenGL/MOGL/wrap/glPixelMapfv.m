function glPixelMapfv( map, mapsize, values )

% glPixelMapfv  Interface to OpenGL function glPixelMapfv
%
% usage:  glPixelMapfv( map, mapsize, values )
%
% C function:  void glPixelMapfv(GLenum map, GLint mapsize, const GLfloat* values)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glPixelMapfv', map, mapsize, moglsingle(values) );

return
