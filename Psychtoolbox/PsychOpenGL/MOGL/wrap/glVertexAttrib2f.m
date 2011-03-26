function glVertexAttrib2f( index, x, y )

% glVertexAttrib2f  Interface to OpenGL function glVertexAttrib2f
%
% usage:  glVertexAttrib2f( index, x, y )
%
% C function:  void glVertexAttrib2f(GLuint index, GLfloat x, GLfloat y)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttrib2f', index, x, y );

return
