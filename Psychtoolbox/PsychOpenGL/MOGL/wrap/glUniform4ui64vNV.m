function glUniform4ui64vNV( location, count, value )

% glUniform4ui64vNV  Interface to OpenGL function glUniform4ui64vNV
%
% usage:  glUniform4ui64vNV( location, count, value )
%
% C function:  void glUniform4ui64vNV(GLint location, GLsizei count, const GLuint64EXT* value)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glUniform4ui64vNV', location, count, uint64(value) );

return
