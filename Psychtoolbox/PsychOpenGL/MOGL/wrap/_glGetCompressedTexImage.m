function img = glGetCompressedTexImage( target, lod )

% glGetCompressedTexImage  Interface to OpenGL function glGetCompressedTexImage
%
% usage:  img = glGetCompressedTexImage( target, lod )
%
% C function:  void glGetCompressedTexImage(GLenum target, GLint lod, GLvoid* img)

% 25-Mar-2011 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

img = (0);

moglcore( 'glGetCompressedTexImage', target, lod, img );

return
