function glVertexAttrib4f( index, x, y, z, w )

% glVertexAttrib4f  Interface to OpenGL function glVertexAttrib4f
%
% usage:  glVertexAttrib4f( index, x, y, z, w )
%
% C function:  void glVertexAttrib4f(GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=5,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttrib4f', index, x, y, z, w );

return
