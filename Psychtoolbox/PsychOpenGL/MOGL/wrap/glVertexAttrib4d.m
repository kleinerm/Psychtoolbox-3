function glVertexAttrib4d( index, x, y, z, w )

% glVertexAttrib4d  Interface to OpenGL function glVertexAttrib4d
%
% usage:  glVertexAttrib4d( index, x, y, z, w )
%
% C function:  void glVertexAttrib4d(GLuint index, GLdouble x, GLdouble y, GLdouble z, GLdouble w)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=5,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttrib4d', index, x, y, z, w );

return
