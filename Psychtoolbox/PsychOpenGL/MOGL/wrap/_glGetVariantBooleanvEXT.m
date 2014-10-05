function data = glGetVariantBooleanvEXT( id, value )

% glGetVariantBooleanvEXT  Interface to OpenGL function glGetVariantBooleanvEXT
%
% usage:  data = glGetVariantBooleanvEXT( id, value )
%
% C function:  void glGetVariantBooleanvEXT(GLuint id, GLenum value, GLboolean* data)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

data = uint8(0);

moglcore( 'glGetVariantBooleanvEXT', id, value, data );

return
