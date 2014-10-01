function glTextureBufferRange( texture, internalformat, buffer, offset, size )

% glTextureBufferRange  Interface to OpenGL function glTextureBufferRange
%
% usage:  glTextureBufferRange( texture, internalformat, buffer, offset, size )
%
% C function:  void glTextureBufferRange(GLuint texture, GLenum internalformat, GLuint buffer, GLintptr offset, GLsizei size)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=5,
    error('invalid number of arguments');
end

moglcore( 'glTextureBufferRange', texture, internalformat, buffer, offset, size );

return
