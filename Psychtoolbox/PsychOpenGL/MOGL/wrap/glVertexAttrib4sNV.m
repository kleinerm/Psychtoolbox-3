function glVertexAttrib4sNV( index, x, y, z, w )

% glVertexAttrib4sNV  Interface to OpenGL function glVertexAttrib4sNV
%
% usage:  glVertexAttrib4sNV( index, x, y, z, w )
%
% C function:  void glVertexAttrib4sNV(GLuint index, GLshort x, GLshort y, GLshort z, GLshort w)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=5,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttrib4sNV', index, x, y, z, w );

return
