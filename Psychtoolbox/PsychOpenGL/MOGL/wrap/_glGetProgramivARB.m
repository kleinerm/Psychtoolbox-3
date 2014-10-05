function params = glGetProgramivARB( target, pname )

% glGetProgramivARB  Interface to OpenGL function glGetProgramivARB
%
% usage:  params = glGetProgramivARB( target, pname )
%
% C function:  void glGetProgramivARB(GLenum target, GLenum pname, GLint* params)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

params = int32(0);

moglcore( 'glGetProgramivARB', target, pname, params );

return
