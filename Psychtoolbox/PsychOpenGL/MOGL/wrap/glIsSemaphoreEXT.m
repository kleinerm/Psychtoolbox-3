function r = glIsSemaphoreEXT( semaphore )

% glIsSemaphoreEXT  Interface to OpenGL function glIsSemaphoreEXT
%
% usage:  r = glIsSemaphoreEXT( semaphore )
%
% C function:  GLboolean glIsSemaphoreEXT(GLuint semaphore)

% 08-Aug-2020 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

r = moglcore( 'glIsSemaphoreEXT', semaphore );

return
