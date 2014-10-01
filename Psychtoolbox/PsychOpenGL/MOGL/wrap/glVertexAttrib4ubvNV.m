function glVertexAttrib4ubvNV( index, v )

% glVertexAttrib4ubvNV  Interface to OpenGL function glVertexAttrib4ubvNV
%
% usage:  glVertexAttrib4ubvNV( index, v )
%
% C function:  void glVertexAttrib4ubvNV(GLuint index, const GLubyte* v)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttrib4ubvNV', index, uint8(v) );

return
