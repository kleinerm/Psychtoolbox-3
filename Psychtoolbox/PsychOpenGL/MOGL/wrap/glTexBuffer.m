function glTexBuffer( target, internalformat, buffer )

% glTexBuffer  Interface to OpenGL function glTexBuffer
%
% usage:  glTexBuffer( target, internalformat, buffer )
%
% C function:  void glTexBuffer(GLenum target, GLenum internalformat, GLuint buffer)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glTexBuffer', target, internalformat, buffer );

return
