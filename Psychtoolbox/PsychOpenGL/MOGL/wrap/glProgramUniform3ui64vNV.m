function glProgramUniform3ui64vNV( program, location, count, value )

% glProgramUniform3ui64vNV  Interface to OpenGL function glProgramUniform3ui64vNV
%
% usage:  glProgramUniform3ui64vNV( program, location, count, value )
%
% C function:  void glProgramUniform3ui64vNV(GLuint program, GLint location, GLsizei count, const GLuint64EXT* value)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glProgramUniform3ui64vNV', program, location, count, uint64(value) );

return
