function glVertexAttrib4usv( index, v )

% glVertexAttrib4usv  Interface to OpenGL function glVertexAttrib4usv
%
% usage:  glVertexAttrib4usv( index, v )
%
% C function:  void glVertexAttrib4usv(GLuint index, const GLushort* v)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttrib4usv', index, uint16(v) );

return
