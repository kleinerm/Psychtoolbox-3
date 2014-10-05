function glVertexAttrib1svNV( index, v )

% glVertexAttrib1svNV  Interface to OpenGL function glVertexAttrib1svNV
%
% usage:  glVertexAttrib1svNV( index, v )
%
% C function:  void glVertexAttrib1svNV(GLuint index, const GLshort* v)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttrib1svNV', index, int16(v) );

return
