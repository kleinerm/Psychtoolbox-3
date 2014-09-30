function glDeleteBuffersARB( n, buffers )

% glDeleteBuffersARB  Interface to OpenGL function glDeleteBuffersARB
%
% usage:  glDeleteBuffersARB( n, buffers )
%
% C function:  void glDeleteBuffersARB(GLsizei n, const GLuint* buffers)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glDeleteBuffersARB', n, uint32(buffers) );

return
