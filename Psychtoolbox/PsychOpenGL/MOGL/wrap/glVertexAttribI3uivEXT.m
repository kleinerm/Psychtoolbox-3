function glVertexAttribI3uivEXT( index, v )

% glVertexAttribI3uivEXT  Interface to OpenGL function glVertexAttribI3uivEXT
%
% usage:  glVertexAttribI3uivEXT( index, v )
%
% C function:  void glVertexAttribI3uivEXT(GLuint index, const GLuint* v)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttribI3uivEXT', index, uint32(v) );

return
