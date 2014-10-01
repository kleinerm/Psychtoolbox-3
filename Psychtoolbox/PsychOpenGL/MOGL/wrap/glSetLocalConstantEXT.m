function glSetLocalConstantEXT( id, type, addr )

% glSetLocalConstantEXT  Interface to OpenGL function glSetLocalConstantEXT
%
% usage:  glSetLocalConstantEXT( id, type, addr )
%
% C function:  void glSetLocalConstantEXT(GLuint id, GLenum type, const void* addr)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glSetLocalConstantEXT', id, type, addr );

return
