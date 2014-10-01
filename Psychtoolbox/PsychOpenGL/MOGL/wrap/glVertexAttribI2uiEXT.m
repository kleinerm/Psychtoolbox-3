function glVertexAttribI2uiEXT( index, x, y )

% glVertexAttribI2uiEXT  Interface to OpenGL function glVertexAttribI2uiEXT
%
% usage:  glVertexAttribI2uiEXT( index, x, y )
%
% C function:  void glVertexAttribI2uiEXT(GLuint index, GLuint x, GLuint y)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttribI2uiEXT', index, x, y );

return
