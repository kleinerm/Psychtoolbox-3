function glVertexAttribI4usvEXT( index, v )

% glVertexAttribI4usvEXT  Interface to OpenGL function glVertexAttribI4usvEXT
%
% usage:  glVertexAttribI4usvEXT( index, v )
%
% C function:  void glVertexAttribI4usvEXT(GLuint index, const GLushort* v)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttribI4usvEXT', index, uint16(v) );

return
