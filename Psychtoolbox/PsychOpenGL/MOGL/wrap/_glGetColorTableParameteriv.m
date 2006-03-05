function params = glGetColorTableParameteriv( target, pname )

% glGetColorTableParameteriv  Interface to OpenGL function glGetColorTableParameteriv
%
% usage:  params = glGetColorTableParameteriv( target, pname )
%
% C function:  void glGetColorTableParameteriv(GLenum target, GLenum pname, GLint* params)

% 05-Mar-2006 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

params = int32(0);

moglcore( 'glGetColorTableParameteriv', target, pname, params );

return
