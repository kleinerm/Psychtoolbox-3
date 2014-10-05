function glVariantusvEXT( id, addr )

% glVariantusvEXT  Interface to OpenGL function glVariantusvEXT
%
% usage:  glVariantusvEXT( id, addr )
%
% C function:  void glVariantusvEXT(GLuint id, const GLushort* addr)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glVariantusvEXT', id, uint16(addr) );

return
