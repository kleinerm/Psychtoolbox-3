function glTextureBufferRangeEXT( texture, target, internalformat, buffer, ptr1, ptr2 )

% glTextureBufferRangeEXT  Interface to OpenGL function glTextureBufferRangeEXT
%
% usage:  glTextureBufferRangeEXT( texture, target, internalformat, buffer, ptr1, ptr2 )
%
% C function:  void glTextureBufferRangeEXT(GLuint texture, GLenum target, GLenum internalformat, GLuint buffer, GLint ptr, GLsizei ptr)

% 30-Aug-2012 -- created (generated automatically from header files)

if nargin~=6,
    error('invalid number of arguments');
end

moglcore( 'glTextureBufferRangeEXT', texture, target, internalformat, buffer, ptr1, ptr2 );

return
