function glProgramUniform3ivEXT( program, location, count, value )

% glProgramUniform3ivEXT  Interface to OpenGL function glProgramUniform3ivEXT
%
% usage:  glProgramUniform3ivEXT( program, location, count, value )
%
% C function:  void glProgramUniform3ivEXT(GLuint program, GLint location, GLsizei count, const GLint* value)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glProgramUniform3ivEXT', program, location, count, int32(value) );

return
