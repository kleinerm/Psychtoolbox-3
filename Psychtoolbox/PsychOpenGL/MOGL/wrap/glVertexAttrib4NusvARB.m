function glVertexAttrib4NusvARB( index, v )

% glVertexAttrib4NusvARB  Interface to OpenGL function glVertexAttrib4NusvARB
%
% usage:  glVertexAttrib4NusvARB( index, v )
%
% C function:  void glVertexAttrib4NusvARB(GLuint index, const GLushort* v)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttrib4NusvARB', index, uint16(v) );

return
