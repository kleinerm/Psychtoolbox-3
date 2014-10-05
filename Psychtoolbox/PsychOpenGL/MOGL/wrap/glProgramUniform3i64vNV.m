function glProgramUniform3i64vNV( program, location, count, value )

% glProgramUniform3i64vNV  Interface to OpenGL function glProgramUniform3i64vNV
%
% usage:  glProgramUniform3i64vNV( program, location, count, value )
%
% C function:  void glProgramUniform3i64vNV(GLuint program, GLint location, GLsizei count, const GLint64EXT* value)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glProgramUniform3i64vNV', program, location, count, int64(value) );

return
