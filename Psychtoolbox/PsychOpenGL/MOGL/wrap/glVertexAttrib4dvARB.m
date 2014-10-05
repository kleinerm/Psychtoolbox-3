function glVertexAttrib4dvARB( index, v )

% glVertexAttrib4dvARB  Interface to OpenGL function glVertexAttrib4dvARB
%
% usage:  glVertexAttrib4dvARB( index, v )
%
% C function:  void glVertexAttrib4dvARB(GLuint index, const GLdouble* v)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttrib4dvARB', index, double(v) );

return
