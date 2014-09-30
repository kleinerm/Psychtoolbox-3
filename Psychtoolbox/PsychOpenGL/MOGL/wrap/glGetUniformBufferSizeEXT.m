function r = glGetUniformBufferSizeEXT( program, location )

% glGetUniformBufferSizeEXT  Interface to OpenGL function glGetUniformBufferSizeEXT
%
% usage:  r = glGetUniformBufferSizeEXT( program, location )
%
% C function:  GLint glGetUniformBufferSizeEXT(GLuint program, GLint location)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

r = moglcore( 'glGetUniformBufferSizeEXT', program, location );

return
