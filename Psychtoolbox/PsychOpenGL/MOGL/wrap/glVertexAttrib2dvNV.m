function glVertexAttrib2dvNV( index, v )

% glVertexAttrib2dvNV  Interface to OpenGL function glVertexAttrib2dvNV
%
% usage:  glVertexAttrib2dvNV( index, v )
%
% C function:  void glVertexAttrib2dvNV(GLuint index, const GLdouble* v)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttrib2dvNV', index, double(v) );

return
