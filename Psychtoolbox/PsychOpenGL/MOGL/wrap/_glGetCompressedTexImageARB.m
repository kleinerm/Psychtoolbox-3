function img = glGetCompressedTexImageARB( target, level )

% glGetCompressedTexImageARB  Interface to OpenGL function glGetCompressedTexImageARB
%
% usage:  img = glGetCompressedTexImageARB( target, level )
%
% C function:  void glGetCompressedTexImageARB(GLenum target, GLint level, void* img)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

img = (0);

moglcore( 'glGetCompressedTexImageARB', target, level, img );

return
