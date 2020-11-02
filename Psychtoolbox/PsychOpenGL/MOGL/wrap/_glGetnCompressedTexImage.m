function pixels = glGetnCompressedTexImage( target, lod, bufSize )

% glGetnCompressedTexImage  Interface to OpenGL function glGetnCompressedTexImage
%
% usage:  pixels = glGetnCompressedTexImage( target, lod, bufSize )
%
% C function:  void glGetnCompressedTexImage(GLenum target, GLint lod, GLsizei bufSize, void* pixels)

% 08-Aug-2020 -- created (generated automatically from header files)

% ---allocate---

if nargin~=3,
    error('invalid number of arguments');
end

pixels = (0);

moglcore( 'glGetnCompressedTexImage', target, lod, bufSize, pixels );

return
