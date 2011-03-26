function glMaterialiv( face, pname, params )

% glMaterialiv  Interface to OpenGL function glMaterialiv
%
% usage:  glMaterialiv( face, pname, params )
%
% C function:  void glMaterialiv(GLenum face, GLenum pname, const GLint* params)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glMaterialiv', face, pname, int32(params) );

return
