function glVertexAttrib2fARB( index, x, y )

% glVertexAttrib2fARB  Interface to OpenGL function glVertexAttrib2fARB
%
% usage:  glVertexAttrib2fARB( index, x, y )
%
% C function:  void glVertexAttrib2fARB(GLuint index, GLfloat x, GLfloat y)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttrib2fARB', index, x, y );

return
