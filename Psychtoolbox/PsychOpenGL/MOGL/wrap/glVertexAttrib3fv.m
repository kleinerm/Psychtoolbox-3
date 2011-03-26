function glVertexAttrib3fv( index, v )

% glVertexAttrib3fv  Interface to OpenGL function glVertexAttrib3fv
%
% usage:  glVertexAttrib3fv( index, v )
%
% C function:  void glVertexAttrib3fv(GLuint index, const GLfloat* v)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttrib3fv', index, single(v) );

return
