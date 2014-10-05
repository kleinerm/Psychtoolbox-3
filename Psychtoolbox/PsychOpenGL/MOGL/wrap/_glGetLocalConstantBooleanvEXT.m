function data = glGetLocalConstantBooleanvEXT( id, value )

% glGetLocalConstantBooleanvEXT  Interface to OpenGL function glGetLocalConstantBooleanvEXT
%
% usage:  data = glGetLocalConstantBooleanvEXT( id, value )
%
% C function:  void glGetLocalConstantBooleanvEXT(GLuint id, GLenum value, GLboolean* data)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

data = uint8(0);

moglcore( 'glGetLocalConstantBooleanvEXT', id, value, data );

return
