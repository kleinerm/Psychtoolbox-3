function glVertexAttribI3ivEXT( index, v )

% glVertexAttribI3ivEXT  Interface to OpenGL function glVertexAttribI3ivEXT
%
% usage:  glVertexAttribI3ivEXT( index, v )
%
% C function:  void glVertexAttribI3ivEXT(GLuint index, const GLint* v)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttribI3ivEXT', index, int32(v) );

return
