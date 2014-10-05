function glVertexAttrib4NubvARB( index, v )

% glVertexAttrib4NubvARB  Interface to OpenGL function glVertexAttrib4NubvARB
%
% usage:  glVertexAttrib4NubvARB( index, v )
%
% C function:  void glVertexAttrib4NubvARB(GLuint index, const GLubyte* v)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttrib4NubvARB', index, uint8(v) );

return
