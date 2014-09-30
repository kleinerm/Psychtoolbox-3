function glTextureBuffer( texture, internalformat, buffer )

% glTextureBuffer  Interface to OpenGL function glTextureBuffer
%
% usage:  glTextureBuffer( texture, internalformat, buffer )
%
% C function:  void glTextureBuffer(GLuint texture, GLenum internalformat, GLuint buffer)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glTextureBuffer', texture, internalformat, buffer );

return
