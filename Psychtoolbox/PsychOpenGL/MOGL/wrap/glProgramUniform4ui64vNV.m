function glProgramUniform4ui64vNV( program, location, count, value )

% glProgramUniform4ui64vNV  Interface to OpenGL function glProgramUniform4ui64vNV
%
% usage:  glProgramUniform4ui64vNV( program, location, count, value )
%
% C function:  void glProgramUniform4ui64vNV(GLuint program, GLint location, GLsizei count, const GLuint64EXT* value)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glProgramUniform4ui64vNV', program, location, count, uint64(value) );

return
