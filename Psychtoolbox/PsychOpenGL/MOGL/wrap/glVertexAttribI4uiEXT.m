function glVertexAttribI4uiEXT( index, x, y, z, w )

% glVertexAttribI4uiEXT  Interface to OpenGL function glVertexAttribI4uiEXT
%
% usage:  glVertexAttribI4uiEXT( index, x, y, z, w )
%
% C function:  void glVertexAttribI4uiEXT(GLuint index, GLuint x, GLuint y, GLuint z, GLuint w)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=5,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttribI4uiEXT', index, x, y, z, w );

return
