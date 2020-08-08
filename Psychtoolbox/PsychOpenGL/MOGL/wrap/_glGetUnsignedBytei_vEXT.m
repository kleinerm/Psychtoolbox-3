function data = glGetUnsignedBytei_vEXT( target, index )

% glGetUnsignedBytei_vEXT  Interface to OpenGL function glGetUnsignedBytei_vEXT
%
% usage:  data = glGetUnsignedBytei_vEXT( target, index )
%
% C function:  void glGetUnsignedBytei_vEXT(GLenum target, GLuint index, GLubyte* data)

% 08-Aug-2020 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

data = uint8(0);

moglcore( 'glGetUnsignedBytei_vEXT', target, index, data );

return
