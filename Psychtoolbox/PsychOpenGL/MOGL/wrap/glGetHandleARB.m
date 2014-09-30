function r = glGetHandleARB( pname )

% glGetHandleARB  Interface to OpenGL function glGetHandleARB
%
% usage:  r = glGetHandleARB( pname )
%
% C function:  GLhandleARB glGetHandleARB(GLenum pname)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

r = moglcore( 'glGetHandleARB', pname );

return
