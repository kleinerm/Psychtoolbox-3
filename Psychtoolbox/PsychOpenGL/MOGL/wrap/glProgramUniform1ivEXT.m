function glProgramUniform1ivEXT( program, location, count, value )

% glProgramUniform1ivEXT  Interface to OpenGL function glProgramUniform1ivEXT
%
% usage:  glProgramUniform1ivEXT( program, location, count, value )
%
% C function:  void glProgramUniform1ivEXT(GLuint program, GLint location, GLsizei count, const GLint* value)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glProgramUniform1ivEXT', program, location, count, int32(value) );

return
