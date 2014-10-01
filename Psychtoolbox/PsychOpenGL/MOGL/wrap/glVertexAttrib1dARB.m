function glVertexAttrib1dARB( index, x )

% glVertexAttrib1dARB  Interface to OpenGL function glVertexAttrib1dARB
%
% usage:  glVertexAttrib1dARB( index, x )
%
% C function:  void glVertexAttrib1dARB(GLuint index, GLdouble x)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttrib1dARB', index, x );

return
