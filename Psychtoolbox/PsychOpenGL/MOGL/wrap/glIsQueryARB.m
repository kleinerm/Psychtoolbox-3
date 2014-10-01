function r = glIsQueryARB( id )

% glIsQueryARB  Interface to OpenGL function glIsQueryARB
%
% usage:  r = glIsQueryARB( id )
%
% C function:  GLboolean glIsQueryARB(GLuint id)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

r = moglcore( 'glIsQueryARB', id );

return
