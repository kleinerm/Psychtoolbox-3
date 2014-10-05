function glDrawBuffersARB( n, bufs )

% glDrawBuffersARB  Interface to OpenGL function glDrawBuffersARB
%
% usage:  glDrawBuffersARB( n, bufs )
%
% C function:  void glDrawBuffersARB(GLsizei n, const GLenum* bufs)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glDrawBuffersARB', n, uint32(bufs) );

return
