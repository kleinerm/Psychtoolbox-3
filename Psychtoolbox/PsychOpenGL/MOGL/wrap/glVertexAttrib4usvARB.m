function glVertexAttrib4usvARB( index, v )

% glVertexAttrib4usvARB  Interface to OpenGL function glVertexAttrib4usvARB
%
% usage:  glVertexAttrib4usvARB( index, v )
%
% C function:  void glVertexAttrib4usvARB(GLuint index, const GLushort* v)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttrib4usvARB', index, uint16(v) );

return
