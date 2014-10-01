function glVertexAttrib2sNV( index, x, y )

% glVertexAttrib2sNV  Interface to OpenGL function glVertexAttrib2sNV
%
% usage:  glVertexAttrib2sNV( index, x, y )
%
% C function:  void glVertexAttrib2sNV(GLuint index, GLshort x, GLshort y)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttrib2sNV', index, x, y );

return
