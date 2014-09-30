function glVertexAttribI4ubvEXT( index, v )

% glVertexAttribI4ubvEXT  Interface to OpenGL function glVertexAttribI4ubvEXT
%
% usage:  glVertexAttribI4ubvEXT( index, v )
%
% C function:  void glVertexAttribI4ubvEXT(GLuint index, const GLubyte* v)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttribI4ubvEXT', index, uint8(v) );

return
