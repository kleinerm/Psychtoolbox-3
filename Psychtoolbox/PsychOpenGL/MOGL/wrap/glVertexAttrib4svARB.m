function glVertexAttrib4svARB( index, v )

% glVertexAttrib4svARB  Interface to OpenGL function glVertexAttrib4svARB
%
% usage:  glVertexAttrib4svARB( index, v )
%
% C function:  void glVertexAttrib4svARB(GLuint index, const GLshort* v)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttrib4svARB', index, int16(v) );

return
