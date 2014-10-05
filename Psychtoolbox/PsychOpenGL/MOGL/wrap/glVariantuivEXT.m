function glVariantuivEXT( id, addr )

% glVariantuivEXT  Interface to OpenGL function glVariantuivEXT
%
% usage:  glVariantuivEXT( id, addr )
%
% C function:  void glVariantuivEXT(GLuint id, const GLuint* addr)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glVariantuivEXT', id, uint32(addr) );

return
