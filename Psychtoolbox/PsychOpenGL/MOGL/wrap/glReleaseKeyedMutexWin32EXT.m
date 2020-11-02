function r = glReleaseKeyedMutexWin32EXT( memory, key )

% glReleaseKeyedMutexWin32EXT  Interface to OpenGL function glReleaseKeyedMutexWin32EXT
%
% usage:  r = glReleaseKeyedMutexWin32EXT( memory, key )
%
% C function:  GLboolean glReleaseKeyedMutexWin32EXT(GLuint memory, GLuint64 key)

% 08-Aug-2020 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

r = moglcore( 'glReleaseKeyedMutexWin32EXT', memory, uint64(key) );

return
