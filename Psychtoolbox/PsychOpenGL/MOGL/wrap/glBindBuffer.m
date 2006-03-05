function glBindBuffer( target, buffer )

% glBindBuffer  Interface to OpenGL function glBindBuffer
%
% usage:  glBindBuffer( target, buffer )
%
% C function:  void glBindBuffer(GLenum target, GLuint buffer)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glBindBuffer', target, buffer );

return
