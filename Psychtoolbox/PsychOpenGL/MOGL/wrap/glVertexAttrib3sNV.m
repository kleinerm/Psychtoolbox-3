function glVertexAttrib3sNV( index, x, y, z )

% glVertexAttrib3sNV  Interface to OpenGL function glVertexAttrib3sNV
%
% usage:  glVertexAttrib3sNV( index, x, y, z )
%
% C function:  void glVertexAttrib3sNV(GLuint index, GLshort x, GLshort y, GLshort z)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttrib3sNV', index, x, y, z );

return
