function glPointParameterfvARB( pname, params )

% glPointParameterfvARB  Interface to OpenGL function glPointParameterfvARB
%
% usage:  glPointParameterfvARB( pname, params )
%
% C function:  void glPointParameterfvARB(GLenum pname, const GLfloat* params)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glPointParameterfvARB', pname, single(params) );

return
