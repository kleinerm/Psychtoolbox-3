function glVertexAttribI4bv( index, v )

% glVertexAttribI4bv  Interface to OpenGL function glVertexAttribI4bv
%
% usage:  glVertexAttribI4bv( index, v )
%
% C function:  void glVertexAttribI4bv(GLuint index, const GLbyte* v)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttribI4bv', index, int8(v) );

return
