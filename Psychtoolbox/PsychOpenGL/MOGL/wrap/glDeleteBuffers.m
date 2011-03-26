function glDeleteBuffers( n, buffers )

% glDeleteBuffers  Interface to OpenGL function glDeleteBuffers
%
% usage:  glDeleteBuffers( n, buffers )
%
% C function:  void glDeleteBuffers(GLsizei n, const GLuint* buffers)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glDeleteBuffers', n, uint32(buffers) );

return
