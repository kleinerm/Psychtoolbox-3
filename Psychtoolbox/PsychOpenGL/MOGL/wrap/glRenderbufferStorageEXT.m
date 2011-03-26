function glRenderbufferStorageEXT( target, internalformat, width, height )

% glRenderbufferStorageEXT  Interface to OpenGL function glRenderbufferStorageEXT
%
% usage:  glRenderbufferStorageEXT( target, internalformat, width, height )
%
% C function:  void glRenderbufferStorageEXT(GLenum target, GLenum internalformat, GLsizei width, GLsizei height)

% 26-Mar-2011 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glRenderbufferStorageEXT', target, internalformat, width, height );

return
