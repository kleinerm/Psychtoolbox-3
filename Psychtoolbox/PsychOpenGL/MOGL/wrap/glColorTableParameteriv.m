function glColorTableParameteriv( target, pname, params )

% glColorTableParameteriv  Interface to OpenGL function glColorTableParameteriv
%
% usage:  glColorTableParameteriv( target, pname, params )
%
% C function:  void glColorTableParameteriv(GLenum target, GLenum pname, const GLint* params)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glColorTableParameteriv', target, pname, int32(params) );

return
