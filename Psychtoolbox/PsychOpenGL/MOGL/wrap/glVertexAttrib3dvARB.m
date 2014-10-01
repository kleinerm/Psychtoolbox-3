function glVertexAttrib3dvARB( index, v )

% glVertexAttrib3dvARB  Interface to OpenGL function glVertexAttrib3dvARB
%
% usage:  glVertexAttrib3dvARB( index, v )
%
% C function:  void glVertexAttrib3dvARB(GLuint index, const GLdouble* v)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttrib3dvARB', index, double(v) );

return
