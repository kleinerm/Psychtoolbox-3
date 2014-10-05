function glTextureBufferEXT( texture, target, internalformat, buffer )

% glTextureBufferEXT  Interface to OpenGL function glTextureBufferEXT
%
% usage:  glTextureBufferEXT( texture, target, internalformat, buffer )
%
% C function:  void glTextureBufferEXT(GLuint texture, GLenum target, GLenum internalformat, GLuint buffer)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glTextureBufferEXT', texture, target, internalformat, buffer );

return
