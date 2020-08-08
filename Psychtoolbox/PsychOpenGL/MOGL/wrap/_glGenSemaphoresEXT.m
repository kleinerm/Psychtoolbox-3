function semaphores = glGenSemaphoresEXT( n )

% glGenSemaphoresEXT  Interface to OpenGL function glGenSemaphoresEXT
%
% usage:  semaphores = glGenSemaphoresEXT( n )
%
% C function:  void glGenSemaphoresEXT(GLsizei n, GLuint* semaphores)

% 08-Aug-2020 -- created (generated automatically from header files)

% ---allocate---

if nargin~=1,
    error('invalid number of arguments');
end

semaphores = uint32(0);

moglcore( 'glGenSemaphoresEXT', n, semaphores );

return
