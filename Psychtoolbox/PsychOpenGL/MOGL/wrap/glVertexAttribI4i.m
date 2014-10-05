function glVertexAttribI4i( index, x, y, z, w )

% glVertexAttribI4i  Interface to OpenGL function glVertexAttribI4i
%
% usage:  glVertexAttribI4i( index, x, y, z, w )
%
% C function:  void glVertexAttribI4i(GLuint index, GLint x, GLint y, GLint z, GLint w)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=5,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttribI4i', index, x, y, z, w );

return
