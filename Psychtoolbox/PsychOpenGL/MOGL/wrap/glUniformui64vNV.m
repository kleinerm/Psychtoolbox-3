function glUniformui64vNV( location, count, value )

% glUniformui64vNV  Interface to OpenGL function glUniformui64vNV
%
% usage:  glUniformui64vNV( location, count, value )
%
% C function:  void glUniformui64vNV(GLint location, GLsizei count, const GLuint64EXT* value)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glUniformui64vNV', location, count, uint64(value) );

return
