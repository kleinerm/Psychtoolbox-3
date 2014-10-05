function params = glGetObjectParameterfvARB( obj, pname )

% glGetObjectParameterfvARB  Interface to OpenGL function glGetObjectParameterfvARB
%
% usage:  params = glGetObjectParameterfvARB( obj, pname )
%
% C function:  void glGetObjectParameterfvARB(GLhandleARB obj, GLenum pname, GLfloat* params)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

params = single(0);

moglcore( 'glGetObjectParameterfvARB', obj, pname, params );

return
