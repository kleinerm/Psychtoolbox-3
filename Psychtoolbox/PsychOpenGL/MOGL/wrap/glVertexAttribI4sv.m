function glVertexAttribI4sv( index, v )

% glVertexAttribI4sv  Interface to OpenGL function glVertexAttribI4sv
%
% usage:  glVertexAttribI4sv( index, v )
%
% C function:  void glVertexAttribI4sv(GLuint index, const GLshort* v)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttribI4sv', index, int16(v) );

return
