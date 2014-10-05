function glVertexAttrib3fARB( index, x, y, z )

% glVertexAttrib3fARB  Interface to OpenGL function glVertexAttrib3fARB
%
% usage:  glVertexAttrib3fARB( index, x, y, z )
%
% C function:  void glVertexAttrib3fARB(GLuint index, GLfloat x, GLfloat y, GLfloat z)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttrib3fARB', index, x, y, z );

return
