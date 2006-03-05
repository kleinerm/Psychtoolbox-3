function glFogiv( pname, params )

% glFogiv  Interface to OpenGL function glFogiv
%
% usage:  glFogiv( pname, params )
%
% C function:  void glFogiv(GLenum pname, const GLint* params)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glFogiv', pname, int32(params) );

return
