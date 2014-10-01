function glVariantfvEXT( id, addr )

% glVariantfvEXT  Interface to OpenGL function glVariantfvEXT
%
% usage:  glVariantfvEXT( id, addr )
%
% C function:  void glVariantfvEXT(GLuint id, const GLfloat* addr)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glVariantfvEXT', id, single(addr) );

return
