function glVertexAttrib2dvARB( index, v )

% glVertexAttrib2dvARB  Interface to OpenGL function glVertexAttrib2dvARB
%
% usage:  glVertexAttrib2dvARB( index, v )
%
% C function:  void glVertexAttrib2dvARB(GLuint index, const GLdouble* v)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttrib2dvARB', index, double(v) );

return
