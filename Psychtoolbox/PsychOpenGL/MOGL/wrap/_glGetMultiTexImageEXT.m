function pixels = glGetMultiTexImageEXT( texunit, target, level, format, type )

% glGetMultiTexImageEXT  Interface to OpenGL function glGetMultiTexImageEXT
%
% usage:  pixels = glGetMultiTexImageEXT( texunit, target, level, format, type )
%
% C function:  void glGetMultiTexImageEXT(GLenum texunit, GLenum target, GLint level, GLenum format, GLenum type, void* pixels)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=5,
    error('invalid number of arguments');
end

pixels = (0);

moglcore( 'glGetMultiTexImageEXT', texunit, target, level, format, type, pixels );

return
