function img = glGetnCompressedTexImageARB( target, lod, bufSize )

% glGetnCompressedTexImageARB  Interface to OpenGL function glGetnCompressedTexImageARB
%
% usage:  img = glGetnCompressedTexImageARB( target, lod, bufSize )
%
% C function:  void glGetnCompressedTexImageARB(GLenum target, GLint lod, GLsizei bufSize, GLvoid* img)

% 30-Aug-2012 -- created (generated automatically from header files)

% ---allocate---

if nargin~=3,
    error('invalid number of arguments');
end

img = (0);

moglcore( 'glGetnCompressedTexImageARB', target, lod, bufSize, img );

return
