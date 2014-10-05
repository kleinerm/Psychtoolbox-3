function glVertexAttrib1dvARB( index, v )

% glVertexAttrib1dvARB  Interface to OpenGL function glVertexAttrib1dvARB
%
% usage:  glVertexAttrib1dvARB( index, v )
%
% C function:  void glVertexAttrib1dvARB(GLuint index, const GLdouble* v)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttrib1dvARB', index, double(v) );

return
