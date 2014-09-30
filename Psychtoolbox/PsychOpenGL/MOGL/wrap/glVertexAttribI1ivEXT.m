function glVertexAttribI1ivEXT( index, v )

% glVertexAttribI1ivEXT  Interface to OpenGL function glVertexAttribI1ivEXT
%
% usage:  glVertexAttribI1ivEXT( index, v )
%
% C function:  void glVertexAttribI1ivEXT(GLuint index, const GLint* v)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttribI1ivEXT', index, int32(v) );

return
