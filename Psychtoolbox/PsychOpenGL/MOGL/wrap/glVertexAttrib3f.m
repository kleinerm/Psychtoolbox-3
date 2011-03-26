function glVertexAttrib3f( index, x, y, z )

% glVertexAttrib3f  Interface to OpenGL function glVertexAttrib3f
%
% usage:  glVertexAttrib3f( index, x, y, z )
%
% C function:  void glVertexAttrib3f(GLuint index, GLfloat x, GLfloat y, GLfloat z)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttrib3f', index, x, y, z );

return
