function glVertexAttrib4sARB( index, x, y, z, w )

% glVertexAttrib4sARB  Interface to OpenGL function glVertexAttrib4sARB
%
% usage:  glVertexAttrib4sARB( index, x, y, z, w )
%
% C function:  void glVertexAttrib4sARB(GLuint index, GLshort x, GLshort y, GLshort z, GLshort w)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=5,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttrib4sARB', index, x, y, z, w );

return
