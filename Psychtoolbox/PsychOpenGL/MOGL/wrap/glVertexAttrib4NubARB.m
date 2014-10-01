function glVertexAttrib4NubARB( index, x, y, z, w )

% glVertexAttrib4NubARB  Interface to OpenGL function glVertexAttrib4NubARB
%
% usage:  glVertexAttrib4NubARB( index, x, y, z, w )
%
% C function:  void glVertexAttrib4NubARB(GLuint index, GLubyte x, GLubyte y, GLubyte z, GLubyte w)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=5,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttrib4NubARB', index, x, y, z, w );

return
