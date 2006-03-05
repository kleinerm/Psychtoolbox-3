function glVertexAttrib4s( index, x, y, z, w )

% glVertexAttrib4s  Interface to OpenGL function glVertexAttrib4s
%
% usage:  glVertexAttrib4s( index, x, y, z, w )
%
% C function:  void glVertexAttrib4s(GLuint index, GLshort x, GLshort y, GLshort z, GLshort w)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=5,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttrib4s', index, x, y, z, w );

return
