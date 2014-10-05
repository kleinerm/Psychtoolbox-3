function glVertexAttribI4ivEXT( index, v )

% glVertexAttribI4ivEXT  Interface to OpenGL function glVertexAttribI4ivEXT
%
% usage:  glVertexAttribI4ivEXT( index, v )
%
% C function:  void glVertexAttribI4ivEXT(GLuint index, const GLint* v)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttribI4ivEXT', index, int32(v) );

return
