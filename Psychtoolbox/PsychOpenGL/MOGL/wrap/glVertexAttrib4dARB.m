function glVertexAttrib4dARB( index, x, y, z, w )

% glVertexAttrib4dARB  Interface to OpenGL function glVertexAttrib4dARB
%
% usage:  glVertexAttrib4dARB( index, x, y, z, w )
%
% C function:  void glVertexAttrib4dARB(GLuint index, GLdouble x, GLdouble y, GLdouble z, GLdouble w)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=5,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttrib4dARB', index, x, y, z, w );

return
