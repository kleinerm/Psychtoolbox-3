function glSetInvariantEXT( id, type, addr )

% glSetInvariantEXT  Interface to OpenGL function glSetInvariantEXT
%
% usage:  glSetInvariantEXT( id, type, addr )
%
% C function:  void glSetInvariantEXT(GLuint id, GLenum type, const void* addr)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glSetInvariantEXT', id, type, addr );

return
