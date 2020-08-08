function glDeleteSemaphoresEXT( n, semaphores )

% glDeleteSemaphoresEXT  Interface to OpenGL function glDeleteSemaphoresEXT
%
% usage:  glDeleteSemaphoresEXT( n, semaphores )
%
% C function:  void glDeleteSemaphoresEXT(GLsizei n, const GLuint* semaphores)

% 08-Aug-2020 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glDeleteSemaphoresEXT', n, uint32(semaphores) );

return
