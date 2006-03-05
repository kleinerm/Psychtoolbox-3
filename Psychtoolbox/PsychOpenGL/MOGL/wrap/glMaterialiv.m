function glMaterialiv( face, pname, params )

% glMaterialiv  Interface to OpenGL function glMaterialiv
%
% usage:  glMaterialiv( face, pname, params )
%
% C function:  void glMaterialiv(GLenum face, GLenum pname, const GLint* params)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glMaterialiv', face, pname, int32(params) );

return
