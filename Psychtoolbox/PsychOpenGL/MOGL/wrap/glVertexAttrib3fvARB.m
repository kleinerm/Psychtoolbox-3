function glVertexAttrib3fvARB( index, v )

% glVertexAttrib3fvARB  Interface to OpenGL function glVertexAttrib3fvARB
%
% usage:  glVertexAttrib3fvARB( index, v )
%
% C function:  void glVertexAttrib3fvARB(GLuint index, const GLfloat* v)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttrib3fvARB', index, single(v) );

return
