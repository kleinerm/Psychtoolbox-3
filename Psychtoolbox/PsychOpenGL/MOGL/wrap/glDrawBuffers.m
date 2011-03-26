function glDrawBuffers( n, bufs )

% glDrawBuffers  Interface to OpenGL function glDrawBuffers
%
% usage:  glDrawBuffers( n, bufs )
%
% C function:  void glDrawBuffers(GLsizei n, const GLenum* bufs)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glDrawBuffers', n, uint32(bufs) );

return
