function glProgramUniform4i64vNV( program, location, count, value )

% glProgramUniform4i64vNV  Interface to OpenGL function glProgramUniform4i64vNV
%
% usage:  glProgramUniform4i64vNV( program, location, count, value )
%
% C function:  void glProgramUniform4i64vNV(GLuint program, GLint location, GLsizei count, const GLint64EXT* value)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glProgramUniform4i64vNV', program, location, count, int64(value) );

return
