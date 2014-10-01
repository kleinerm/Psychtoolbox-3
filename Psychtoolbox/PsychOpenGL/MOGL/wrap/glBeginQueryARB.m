function glBeginQueryARB( target, id )

% glBeginQueryARB  Interface to OpenGL function glBeginQueryARB
%
% usage:  glBeginQueryARB( target, id )
%
% C function:  void glBeginQueryARB(GLenum target, GLuint id)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glBeginQueryARB', target, id );

return
