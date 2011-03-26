function glPixelMapuiv( map, mapsize, values )

% glPixelMapuiv  Interface to OpenGL function glPixelMapuiv
%
% usage:  glPixelMapuiv( map, mapsize, values )
%
% C function:  void glPixelMapuiv(GLenum map, GLint mapsize, const GLuint* values)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glPixelMapuiv', map, mapsize, uint32(values) );

return
