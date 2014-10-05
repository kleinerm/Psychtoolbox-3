function glProgramUniform2ivEXT( program, location, count, value )

% glProgramUniform2ivEXT  Interface to OpenGL function glProgramUniform2ivEXT
%
% usage:  glProgramUniform2ivEXT( program, location, count, value )
%
% C function:  void glProgramUniform2ivEXT(GLuint program, GLint location, GLsizei count, const GLint* value)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glProgramUniform2ivEXT', program, location, count, int32(value) );

return
