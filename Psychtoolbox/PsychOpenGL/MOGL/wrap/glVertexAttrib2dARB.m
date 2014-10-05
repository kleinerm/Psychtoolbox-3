function glVertexAttrib2dARB( index, x, y )

% glVertexAttrib2dARB  Interface to OpenGL function glVertexAttrib2dARB
%
% usage:  glVertexAttrib2dARB( index, x, y )
%
% C function:  void glVertexAttrib2dARB(GLuint index, GLdouble x, GLdouble y)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttrib2dARB', index, x, y );

return
