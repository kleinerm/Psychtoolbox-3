function glVertexAttribI3ui( index, x, y, z )

% glVertexAttribI3ui  Interface to OpenGL function glVertexAttribI3ui
%
% usage:  glVertexAttribI3ui( index, x, y, z )
%
% C function:  void glVertexAttribI3ui(GLuint index, GLuint x, GLuint y, GLuint z)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttribI3ui', index, x, y, z );

return
