function glVertexAttribI2uivEXT( index, v )

% glVertexAttribI2uivEXT  Interface to OpenGL function glVertexAttribI2uivEXT
%
% usage:  glVertexAttribI2uivEXT( index, v )
%
% C function:  void glVertexAttribI2uivEXT(GLuint index, const GLuint* v)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttribI2uivEXT', index, uint32(v) );

return
