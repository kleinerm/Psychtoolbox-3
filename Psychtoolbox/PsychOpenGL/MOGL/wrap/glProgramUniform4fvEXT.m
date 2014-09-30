function glProgramUniform4fvEXT( program, location, count, value )

% glProgramUniform4fvEXT  Interface to OpenGL function glProgramUniform4fvEXT
%
% usage:  glProgramUniform4fvEXT( program, location, count, value )
%
% C function:  void glProgramUniform4fvEXT(GLuint program, GLint location, GLsizei count, const GLfloat* value)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glProgramUniform4fvEXT', program, location, count, single(value) );

return
