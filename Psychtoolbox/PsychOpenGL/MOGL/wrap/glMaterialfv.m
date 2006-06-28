function glMaterialfv( face, pname, params )

% glMaterialfv  Interface to OpenGL function glMaterialfv
%
% usage:  glMaterialfv( face, pname, params )
%
% C function:  void glMaterialfv(GLenum face, GLenum pname, const GLfloat* params)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glMaterialfv', face, pname, moglsingle(params) );

return
