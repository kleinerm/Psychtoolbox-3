function glBindBufferARB( target, buffer )

% glBindBufferARB  Interface to OpenGL function glBindBufferARB
%
% usage:  glBindBufferARB( target, buffer )
%
% C function:  void glBindBufferARB(GLenum target, GLuint buffer)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glBindBufferARB', target, buffer );

return
