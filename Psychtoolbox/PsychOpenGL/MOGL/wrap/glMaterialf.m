function glMaterialf( face, pname, param )

% glMaterialf  Interface to OpenGL function glMaterialf
%
% usage:  glMaterialf( face, pname, param )
%
% C function:  void glMaterialf(GLenum face, GLenum pname, GLfloat param)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glMaterialf', face, pname, param );

return
