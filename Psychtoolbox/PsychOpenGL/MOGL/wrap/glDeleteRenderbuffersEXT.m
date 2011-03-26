function glDeleteRenderbuffersEXT( n, renderbuffers )

% glDeleteRenderbuffersEXT  Interface to OpenGL function glDeleteRenderbuffersEXT
%
% usage:  glDeleteRenderbuffersEXT( n, renderbuffers )
%
% C function:  void glDeleteRenderbuffersEXT(GLsizei n, const GLuint* renderbuffers)

% 26-Mar-2011 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glDeleteRenderbuffersEXT', n, uint32(renderbuffers) );

return
