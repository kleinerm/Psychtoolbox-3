function glUniform1i64vNV( location, count, value )

% glUniform1i64vNV  Interface to OpenGL function glUniform1i64vNV
%
% usage:  glUniform1i64vNV( location, count, value )
%
% C function:  void glUniform1i64vNV(GLint location, GLsizei count, const GLint64EXT* value)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glUniform1i64vNV', location, count, int64(value) );

return
