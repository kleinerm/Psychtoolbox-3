function glVertexAttribI4usv( index, v )

% glVertexAttribI4usv  Interface to OpenGL function glVertexAttribI4usv
%
% usage:  glVertexAttribI4usv( index, v )
%
% C function:  void glVertexAttribI4usv(GLuint index, const GLushort* v)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttribI4usv', index, uint16(v) );

return
