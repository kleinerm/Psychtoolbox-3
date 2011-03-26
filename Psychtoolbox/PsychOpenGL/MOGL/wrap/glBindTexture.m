function glBindTexture( target, texture )

% glBindTexture  Interface to OpenGL function glBindTexture
%
% usage:  glBindTexture( target, texture )
%
% C function:  void glBindTexture(GLenum target, GLuint texture)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glBindTexture', target, texture );

return
