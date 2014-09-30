function glVertexAttrib1fvARB( index, v )

% glVertexAttrib1fvARB  Interface to OpenGL function glVertexAttrib1fvARB
%
% usage:  glVertexAttrib1fvARB( index, v )
%
% C function:  void glVertexAttrib1fvARB(GLuint index, const GLfloat* v)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttrib1fvARB', index, single(v) );

return
