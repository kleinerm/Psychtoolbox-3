function glVertexAttribI3i( index, x, y, z )

% glVertexAttribI3i  Interface to OpenGL function glVertexAttribI3i
%
% usage:  glVertexAttribI3i( index, x, y, z )
%
% C function:  void glVertexAttribI3i(GLuint index, GLint x, GLint y, GLint z)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttribI3i', index, x, y, z );

return
