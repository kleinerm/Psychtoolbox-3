function glVertexAttrib1sNV( index, x )

% glVertexAttrib1sNV  Interface to OpenGL function glVertexAttrib1sNV
%
% usage:  glVertexAttrib1sNV( index, x )
%
% C function:  void glVertexAttrib1sNV(GLuint index, GLshort x)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttrib1sNV', index, x );

return
