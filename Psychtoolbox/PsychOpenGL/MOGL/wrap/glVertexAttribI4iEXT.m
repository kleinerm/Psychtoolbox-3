function glVertexAttribI4iEXT( index, x, y, z, w )

% glVertexAttribI4iEXT  Interface to OpenGL function glVertexAttribI4iEXT
%
% usage:  glVertexAttribI4iEXT( index, x, y, z, w )
%
% C function:  void glVertexAttribI4iEXT(GLuint index, GLint x, GLint y, GLint z, GLint w)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=5,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttribI4iEXT', index, x, y, z, w );

return
