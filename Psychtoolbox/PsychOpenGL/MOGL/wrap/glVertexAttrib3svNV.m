function glVertexAttrib3svNV( index, v )

% glVertexAttrib3svNV  Interface to OpenGL function glVertexAttrib3svNV
%
% usage:  glVertexAttrib3svNV( index, v )
%
% C function:  void glVertexAttrib3svNV(GLuint index, const GLshort* v)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttrib3svNV', index, int16(v) );

return
