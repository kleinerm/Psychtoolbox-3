function glVertexAttrib1fv( index, v )

% glVertexAttrib1fv  Interface to OpenGL function glVertexAttrib1fv
%
% usage:  glVertexAttrib1fv( index, v )
%
% C function:  void glVertexAttrib1fv(GLuint index, const GLfloat* v)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttrib1fv', index, single(v) );

return
