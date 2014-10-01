function glVertexAttrib4ubNV( index, x, y, z, w )

% glVertexAttrib4ubNV  Interface to OpenGL function glVertexAttrib4ubNV
%
% usage:  glVertexAttrib4ubNV( index, x, y, z, w )
%
% C function:  void glVertexAttrib4ubNV(GLuint index, GLubyte x, GLubyte y, GLubyte z, GLubyte w)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=5,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttrib4ubNV', index, x, y, z, w );

return
