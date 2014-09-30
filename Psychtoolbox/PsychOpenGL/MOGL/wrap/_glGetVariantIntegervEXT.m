function data = glGetVariantIntegervEXT( id, value )

% glGetVariantIntegervEXT  Interface to OpenGL function glGetVariantIntegervEXT
%
% usage:  data = glGetVariantIntegervEXT( id, value )
%
% C function:  void glGetVariantIntegervEXT(GLuint id, GLenum value, GLint* data)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

data = int32(0);

moglcore( 'glGetVariantIntegervEXT', id, value, data );

return
