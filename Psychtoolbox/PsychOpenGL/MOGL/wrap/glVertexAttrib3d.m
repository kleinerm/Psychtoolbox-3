function glVertexAttrib3d( index, x, y, z )

% glVertexAttrib3d  Interface to OpenGL function glVertexAttrib3d
%
% usage:  glVertexAttrib3d( index, x, y, z )
%
% C function:  void glVertexAttrib3d(GLuint index, GLdouble x, GLdouble y, GLdouble z)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttrib3d', index, x, y, z );

return
