function glVertexAttrib4svNV( index, v )

% glVertexAttrib4svNV  Interface to OpenGL function glVertexAttrib4svNV
%
% usage:  glVertexAttrib4svNV( index, v )
%
% C function:  void glVertexAttrib4svNV(GLuint index, const GLshort* v)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttrib4svNV', index, int16(v) );

return
