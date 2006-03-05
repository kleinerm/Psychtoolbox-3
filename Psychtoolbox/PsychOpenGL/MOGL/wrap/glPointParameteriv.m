function glPointParameteriv( pname, params )

% glPointParameteriv  Interface to OpenGL function glPointParameteriv
%
% usage:  glPointParameteriv( pname, params )
%
% C function:  void glPointParameteriv(GLenum pname, const GLint* params)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glPointParameteriv', pname, int32(params) );

return
