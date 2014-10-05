function glVertexAttrib4dvNV( index, v )

% glVertexAttrib4dvNV  Interface to OpenGL function glVertexAttrib4dvNV
%
% usage:  glVertexAttrib4dvNV( index, v )
%
% C function:  void glVertexAttrib4dvNV(GLuint index, const GLdouble* v)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttrib4dvNV', index, double(v) );

return
