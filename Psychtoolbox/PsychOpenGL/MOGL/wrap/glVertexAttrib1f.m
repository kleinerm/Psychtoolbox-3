function glVertexAttrib1f( index, x )

% glVertexAttrib1f  Interface to OpenGL function glVertexAttrib1f
%
% usage:  glVertexAttrib1f( index, x )
%
% C function:  void glVertexAttrib1f(GLuint index, GLfloat x)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttrib1f', index, x );

return
