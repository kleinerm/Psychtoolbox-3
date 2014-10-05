function glVariantivEXT( id, addr )

% glVariantivEXT  Interface to OpenGL function glVariantivEXT
%
% usage:  glVariantivEXT( id, addr )
%
% C function:  void glVariantivEXT(GLuint id, const GLint* addr)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glVariantivEXT', id, int32(addr) );

return
