function r = glIsBufferARB( buffer )

% glIsBufferARB  Interface to OpenGL function glIsBufferARB
%
% usage:  r = glIsBufferARB( buffer )
%
% C function:  GLboolean glIsBufferARB(GLuint buffer)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

r = moglcore( 'glIsBufferARB', buffer );

return
