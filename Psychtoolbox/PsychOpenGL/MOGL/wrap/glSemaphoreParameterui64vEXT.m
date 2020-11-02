function glSemaphoreParameterui64vEXT( semaphore, pname, params )

% glSemaphoreParameterui64vEXT  Interface to OpenGL function glSemaphoreParameterui64vEXT
%
% usage:  glSemaphoreParameterui64vEXT( semaphore, pname, params )
%
% C function:  void glSemaphoreParameterui64vEXT(GLuint semaphore, GLenum pname, const GLuint64* params)

% 08-Aug-2020 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glSemaphoreParameterui64vEXT', semaphore, pname, uint64(params) );

return
