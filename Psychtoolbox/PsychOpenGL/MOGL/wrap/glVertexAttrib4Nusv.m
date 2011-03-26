function glVertexAttrib4Nusv( index, v )

% glVertexAttrib4Nusv  Interface to OpenGL function glVertexAttrib4Nusv
%
% usage:  glVertexAttrib4Nusv( index, v )
%
% C function:  void glVertexAttrib4Nusv(GLuint index, const GLushort* v)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttrib4Nusv', index, uint16(v) );

return
