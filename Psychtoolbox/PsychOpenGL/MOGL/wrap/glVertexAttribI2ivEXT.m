function glVertexAttribI2ivEXT( index, v )

% glVertexAttribI2ivEXT  Interface to OpenGL function glVertexAttribI2ivEXT
%
% usage:  glVertexAttribI2ivEXT( index, v )
%
% C function:  void glVertexAttribI2ivEXT(GLuint index, const GLint* v)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttribI2ivEXT', index, int32(v) );

return
