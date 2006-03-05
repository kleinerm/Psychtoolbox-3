function glTexEnviv( target, pname, params )

% glTexEnviv  Interface to OpenGL function glTexEnviv
%
% usage:  glTexEnviv( target, pname, params )
%
% C function:  void glTexEnviv(GLenum target, GLenum pname, const GLint* params)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glTexEnviv', target, pname, int32(params) );

return
