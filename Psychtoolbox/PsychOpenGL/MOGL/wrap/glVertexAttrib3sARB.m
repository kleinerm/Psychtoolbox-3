function glVertexAttrib3sARB( index, x, y, z )

% glVertexAttrib3sARB  Interface to OpenGL function glVertexAttrib3sARB
%
% usage:  glVertexAttrib3sARB( index, x, y, z )
%
% C function:  void glVertexAttrib3sARB(GLuint index, GLshort x, GLshort y, GLshort z)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttrib3sARB', index, x, y, z );

return
