function glVertexAttrib1fARB( index, x )

% glVertexAttrib1fARB  Interface to OpenGL function glVertexAttrib1fARB
%
% usage:  glVertexAttrib1fARB( index, x )
%
% C function:  void glVertexAttrib1fARB(GLuint index, GLfloat x)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttrib1fARB', index, x );

return
