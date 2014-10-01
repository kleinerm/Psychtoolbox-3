function glVertexAttribI1uivEXT( index, v )

% glVertexAttribI1uivEXT  Interface to OpenGL function glVertexAttribI1uivEXT
%
% usage:  glVertexAttribI1uivEXT( index, v )
%
% C function:  void glVertexAttribI1uivEXT(GLuint index, const GLuint* v)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttribI1uivEXT', index, uint32(v) );

return
