function glVertexAttrib3s( index, x, y, z )

% glVertexAttrib3s  Interface to OpenGL function glVertexAttrib3s
%
% usage:  glVertexAttrib3s( index, x, y, z )
%
% C function:  void glVertexAttrib3s(GLuint index, GLshort x, GLshort y, GLshort z)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttrib3s', index, x, y, z );

return
