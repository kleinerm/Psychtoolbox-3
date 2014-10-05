function params = glGetObjectParameterivARB( obj, pname )

% glGetObjectParameterivARB  Interface to OpenGL function glGetObjectParameterivARB
%
% usage:  params = glGetObjectParameterivARB( obj, pname )
%
% C function:  void glGetObjectParameterivARB(GLhandleARB obj, GLenum pname, GLint* params)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

params = int32(0);

moglcore( 'glGetObjectParameterivARB', obj, pname, params );

return
