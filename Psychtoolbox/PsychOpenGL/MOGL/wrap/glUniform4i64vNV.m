function glUniform4i64vNV( location, count, value )

% glUniform4i64vNV  Interface to OpenGL function glUniform4i64vNV
%
% usage:  glUniform4i64vNV( location, count, value )
%
% C function:  void glUniform4i64vNV(GLint location, GLsizei count, const GLint64EXT* value)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glUniform4i64vNV', location, count, int64(value) );

return
