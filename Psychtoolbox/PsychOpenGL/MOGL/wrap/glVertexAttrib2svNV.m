function glVertexAttrib2svNV( index, v )

% glVertexAttrib2svNV  Interface to OpenGL function glVertexAttrib2svNV
%
% usage:  glVertexAttrib2svNV( index, v )
%
% C function:  void glVertexAttrib2svNV(GLuint index, const GLshort* v)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttrib2svNV', index, int16(v) );

return
