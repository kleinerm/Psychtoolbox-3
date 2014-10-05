function glVertexAttrib4fvARB( index, v )

% glVertexAttrib4fvARB  Interface to OpenGL function glVertexAttrib4fvARB
%
% usage:  glVertexAttrib4fvARB( index, v )
%
% C function:  void glVertexAttrib4fvARB(GLuint index, const GLfloat* v)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttrib4fvARB', index, single(v) );

return
