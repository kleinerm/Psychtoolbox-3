function glProgramUniformMatrix2fvEXT( program, location, count, transpose, value )

% glProgramUniformMatrix2fvEXT  Interface to OpenGL function glProgramUniformMatrix2fvEXT
%
% usage:  glProgramUniformMatrix2fvEXT( program, location, count, transpose, value )
%
% C function:  void glProgramUniformMatrix2fvEXT(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=5,
    error('invalid number of arguments');
end

moglcore( 'glProgramUniformMatrix2fvEXT', program, location, count, transpose, single(value) );

return
