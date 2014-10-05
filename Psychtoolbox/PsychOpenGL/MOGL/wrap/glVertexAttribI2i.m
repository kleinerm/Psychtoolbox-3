function glVertexAttribI2i( index, x, y )

% glVertexAttribI2i  Interface to OpenGL function glVertexAttribI2i
%
% usage:  glVertexAttribI2i( index, x, y )
%
% C function:  void glVertexAttribI2i(GLuint index, GLint x, GLint y)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttribI2i', index, x, y );

return
