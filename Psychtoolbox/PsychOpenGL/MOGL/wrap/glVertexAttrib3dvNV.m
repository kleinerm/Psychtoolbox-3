function glVertexAttrib3dvNV( index, v )

% glVertexAttrib3dvNV  Interface to OpenGL function glVertexAttrib3dvNV
%
% usage:  glVertexAttrib3dvNV( index, v )
%
% C function:  void glVertexAttrib3dvNV(GLuint index, const GLdouble* v)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttrib3dvNV', index, double(v) );

return
