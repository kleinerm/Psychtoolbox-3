function glVertexAttribI4uivEXT( index, v )

% glVertexAttribI4uivEXT  Interface to OpenGL function glVertexAttribI4uivEXT
%
% usage:  glVertexAttribI4uivEXT( index, v )
%
% C function:  void glVertexAttribI4uivEXT(GLuint index, const GLuint* v)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttribI4uivEXT', index, uint32(v) );

return
