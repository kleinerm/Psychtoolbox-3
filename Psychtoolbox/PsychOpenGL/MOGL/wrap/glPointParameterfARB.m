function glPointParameterfARB( pname, param )

% glPointParameterfARB  Interface to OpenGL function glPointParameterfARB
%
% usage:  glPointParameterfARB( pname, param )
%
% C function:  void glPointParameterfARB(GLenum pname, GLfloat param)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glPointParameterfARB', pname, param );

return
