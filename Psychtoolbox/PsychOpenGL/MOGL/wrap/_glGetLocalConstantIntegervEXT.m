function data = glGetLocalConstantIntegervEXT( id, value )

% glGetLocalConstantIntegervEXT  Interface to OpenGL function glGetLocalConstantIntegervEXT
%
% usage:  data = glGetLocalConstantIntegervEXT( id, value )
%
% C function:  void glGetLocalConstantIntegervEXT(GLuint id, GLenum value, GLint* data)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

data = int32(0);

moglcore( 'glGetLocalConstantIntegervEXT', id, value, data );

return
