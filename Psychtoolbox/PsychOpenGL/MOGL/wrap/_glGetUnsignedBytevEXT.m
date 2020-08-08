function data = glGetUnsignedBytevEXT( pname )

% glGetUnsignedBytevEXT  Interface to OpenGL function glGetUnsignedBytevEXT
%
% usage:  data = glGetUnsignedBytevEXT( pname )
%
% C function:  void glGetUnsignedBytevEXT(GLenum pname, GLubyte* data)

% 08-Aug-2020 -- created (generated automatically from header files)

% ---allocate---

if nargin~=1,
    error('invalid number of arguments');
end

data = uint8(0);

moglcore( 'glGetUnsignedBytevEXT', pname, data );

return
