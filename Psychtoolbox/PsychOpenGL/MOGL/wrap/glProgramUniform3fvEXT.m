function glProgramUniform3fvEXT( program, location, count, value )

% glProgramUniform3fvEXT  Interface to OpenGL function glProgramUniform3fvEXT
%
% usage:  glProgramUniform3fvEXT( program, location, count, value )
%
% C function:  void glProgramUniform3fvEXT(GLuint program, GLint location, GLsizei count, const GLfloat* value)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glProgramUniform3fvEXT', program, location, count, single(value) );

return
