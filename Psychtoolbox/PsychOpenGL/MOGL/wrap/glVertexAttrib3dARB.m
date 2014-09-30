function glVertexAttrib3dARB( index, x, y, z )

% glVertexAttrib3dARB  Interface to OpenGL function glVertexAttrib3dARB
%
% usage:  glVertexAttrib3dARB( index, x, y, z )
%
% C function:  void glVertexAttrib3dARB(GLuint index, GLdouble x, GLdouble y, GLdouble z)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttrib3dARB', index, x, y, z );

return
