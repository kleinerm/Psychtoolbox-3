function glVertexAttribI4svEXT( index, v )

% glVertexAttribI4svEXT  Interface to OpenGL function glVertexAttribI4svEXT
%
% usage:  glVertexAttribI4svEXT( index, v )
%
% C function:  void glVertexAttribI4svEXT(GLuint index, const GLshort* v)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttribI4svEXT', index, int16(v) );

return
