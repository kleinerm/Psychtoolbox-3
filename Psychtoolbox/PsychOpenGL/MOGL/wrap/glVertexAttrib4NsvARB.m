function glVertexAttrib4NsvARB( index, v )

% glVertexAttrib4NsvARB  Interface to OpenGL function glVertexAttrib4NsvARB
%
% usage:  glVertexAttrib4NsvARB( index, v )
%
% C function:  void glVertexAttrib4NsvARB(GLuint index, const GLshort* v)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttrib4NsvARB', index, int16(v) );

return
