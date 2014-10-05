function glVertexAttribI3iEXT( index, x, y, z )

% glVertexAttribI3iEXT  Interface to OpenGL function glVertexAttribI3iEXT
%
% usage:  glVertexAttribI3iEXT( index, x, y, z )
%
% C function:  void glVertexAttribI3iEXT(GLuint index, GLint x, GLint y, GLint z)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttribI3iEXT', index, x, y, z );

return
