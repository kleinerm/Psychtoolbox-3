function glColorTableEXT( target, internalFormat, width, format, type, table )

% glColorTableEXT  Interface to OpenGL function glColorTableEXT
%
% usage:  glColorTableEXT( target, internalFormat, width, format, type, table )
%
% C function:  void glColorTableEXT(GLenum target, GLenum internalFormat, GLsizei width, GLenum format, GLenum type, const void* table)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=6,
    error('invalid number of arguments');
end

moglcore( 'glColorTableEXT', target, internalFormat, width, format, type, table );

return
