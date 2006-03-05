function glLightModeliv( pname, params )

% glLightModeliv  Interface to OpenGL function glLightModeliv
%
% usage:  glLightModeliv( pname, params )
%
% C function:  void glLightModeliv(GLenum pname, const GLint* params)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glLightModeliv', pname, int32(params) );

return
