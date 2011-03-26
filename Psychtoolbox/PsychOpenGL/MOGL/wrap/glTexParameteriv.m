function glTexParameteriv( target, pname, params )

% glTexParameteriv  Interface to OpenGL function glTexParameteriv
%
% usage:  glTexParameteriv( target, pname, params )
%
% C function:  void glTexParameteriv(GLenum target, GLenum pname, const GLint* params)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glTexParameteriv', target, pname, int32(params) );

return
