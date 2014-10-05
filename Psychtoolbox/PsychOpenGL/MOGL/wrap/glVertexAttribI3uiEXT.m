function glVertexAttribI3uiEXT( index, x, y, z )

% glVertexAttribI3uiEXT  Interface to OpenGL function glVertexAttribI3uiEXT
%
% usage:  glVertexAttribI3uiEXT( index, x, y, z )
%
% C function:  void glVertexAttribI3uiEXT(GLuint index, GLuint x, GLuint y, GLuint z)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttribI3uiEXT', index, x, y, z );

return
