function params = glGetMinmaxParameteriv( target, pname )

% glGetMinmaxParameteriv  Interface to OpenGL function glGetMinmaxParameteriv
%
% usage:  params = glGetMinmaxParameteriv( target, pname )
%
% C function:  void glGetMinmaxParameteriv(GLenum target, GLenum pname, GLint* params)

% 05-Mar-2006 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

params = int32(0);

moglcore( 'glGetMinmaxParameteriv', target, pname, params );

return
