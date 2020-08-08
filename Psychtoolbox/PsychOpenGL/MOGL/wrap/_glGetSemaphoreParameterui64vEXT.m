function params = glGetSemaphoreParameterui64vEXT( semaphore, pname )

% glGetSemaphoreParameterui64vEXT  Interface to OpenGL function glGetSemaphoreParameterui64vEXT
%
% usage:  params = glGetSemaphoreParameterui64vEXT( semaphore, pname )
%
% C function:  void glGetSemaphoreParameterui64vEXT(GLuint semaphore, GLenum pname, GLuint64* params)

% 08-Aug-2020 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

params = uint64(0);

moglcore( 'glGetSemaphoreParameterui64vEXT', semaphore, pname, params );

return
