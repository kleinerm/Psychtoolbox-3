function glVertexAttribI4ui( index, x, y, z, w )

% glVertexAttribI4ui  Interface to OpenGL function glVertexAttribI4ui
%
% usage:  glVertexAttribI4ui( index, x, y, z, w )
%
% C function:  void glVertexAttribI4ui(GLuint index, GLuint x, GLuint y, GLuint z, GLuint w)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=5,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttribI4ui', index, x, y, z, w );

return
