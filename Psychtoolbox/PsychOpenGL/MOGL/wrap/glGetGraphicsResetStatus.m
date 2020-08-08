function r = glGetGraphicsResetStatus

% glGetGraphicsResetStatus  Interface to OpenGL function glGetGraphicsResetStatus
%
% usage:  r = glGetGraphicsResetStatus
%
% C function:  GLenum glGetGraphicsResetStatus(void)

% 08-Aug-2020 -- created (generated automatically from header files)

if nargin~=0,
    error('invalid number of arguments');
end

r = moglcore( 'glGetGraphicsResetStatus' );

return
