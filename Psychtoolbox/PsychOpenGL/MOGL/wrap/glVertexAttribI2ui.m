function glVertexAttribI2ui( index, x, y )

% glVertexAttribI2ui  Interface to OpenGL function glVertexAttribI2ui
%
% usage:  glVertexAttribI2ui( index, x, y )
%
% C function:  void glVertexAttribI2ui(GLuint index, GLuint x, GLuint y)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttribI2ui', index, x, y );

return
