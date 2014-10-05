function img = glGetCompressedMultiTexImageEXT( texunit, target, lod )

% glGetCompressedMultiTexImageEXT  Interface to OpenGL function glGetCompressedMultiTexImageEXT
%
% usage:  img = glGetCompressedMultiTexImageEXT( texunit, target, lod )
%
% C function:  void glGetCompressedMultiTexImageEXT(GLenum texunit, GLenum target, GLint lod, void* img)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=3,
    error('invalid number of arguments');
end

img = (0);

moglcore( 'glGetCompressedMultiTexImageEXT', texunit, target, lod, img );

return
