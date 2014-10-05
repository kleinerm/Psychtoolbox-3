function glUniformBufferEXT( program, location, buffer )

% glUniformBufferEXT  Interface to OpenGL function glUniformBufferEXT
%
% usage:  glUniformBufferEXT( program, location, buffer )
%
% C function:  void glUniformBufferEXT(GLuint program, GLint location, GLuint buffer)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glUniformBufferEXT', program, location, buffer );

return
