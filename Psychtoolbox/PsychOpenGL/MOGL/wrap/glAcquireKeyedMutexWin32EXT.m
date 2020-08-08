function r = glAcquireKeyedMutexWin32EXT( memory, key, timeout )

% glAcquireKeyedMutexWin32EXT  Interface to OpenGL function glAcquireKeyedMutexWin32EXT
%
% usage:  r = glAcquireKeyedMutexWin32EXT( memory, key, timeout )
%
% C function:  GLboolean glAcquireKeyedMutexWin32EXT(GLuint memory, GLuint64 key, GLuint timeout)

% 08-Aug-2020 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

r = moglcore( 'glAcquireKeyedMutexWin32EXT', memory, uint64(key), timeout );

return
