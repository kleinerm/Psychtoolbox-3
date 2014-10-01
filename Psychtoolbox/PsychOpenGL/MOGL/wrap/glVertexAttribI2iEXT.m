function glVertexAttribI2iEXT( index, x, y )

% glVertexAttribI2iEXT  Interface to OpenGL function glVertexAttribI2iEXT
%
% usage:  glVertexAttribI2iEXT( index, x, y )
%
% C function:  void glVertexAttribI2iEXT(GLuint index, GLint x, GLint y)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttribI2iEXT', index, x, y );

return
