function glTexBufferEXT( target, internalformat, buffer )

% glTexBufferEXT  Interface to OpenGL function glTexBufferEXT
%
% usage:  glTexBufferEXT( target, internalformat, buffer )
%
% C function:  void glTexBufferEXT(GLenum target, GLenum internalformat, GLuint buffer)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glTexBufferEXT', target, internalformat, buffer );

return
