function glVertexAttrib1fNV( index, x )

% glVertexAttrib1fNV  Interface to OpenGL function glVertexAttrib1fNV
%
% usage:  glVertexAttrib1fNV( index, x )
%
% C function:  void glVertexAttrib1fNV(GLuint index, GLfloat x)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttrib1fNV', index, x );

return
