function glProgramUniform4ivEXT( program, location, count, value )

% glProgramUniform4ivEXT  Interface to OpenGL function glProgramUniform4ivEXT
%
% usage:  glProgramUniform4ivEXT( program, location, count, value )
%
% C function:  void glProgramUniform4ivEXT(GLuint program, GLint location, GLsizei count, const GLint* value)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glProgramUniform4ivEXT', program, location, count, int32(value) );

return
