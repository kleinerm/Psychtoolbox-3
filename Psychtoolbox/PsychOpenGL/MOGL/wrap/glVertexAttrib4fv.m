function glVertexAttrib4fv( index, v )

% glVertexAttrib4fv  Interface to OpenGL function glVertexAttrib4fv
%
% usage:  glVertexAttrib4fv( index, v )
%
% C function:  void glVertexAttrib4fv(GLuint index, const GLfloat* v)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttrib4fv', index, single(v) );

return
