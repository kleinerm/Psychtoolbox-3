function params = glGetQueryivARB( target, pname )

% glGetQueryivARB  Interface to OpenGL function glGetQueryivARB
%
% usage:  params = glGetQueryivARB( target, pname )
%
% C function:  void glGetQueryivARB(GLenum target, GLenum pname, GLint* params)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

params = int32(0);

moglcore( 'glGetQueryivARB', target, pname, params );

return
