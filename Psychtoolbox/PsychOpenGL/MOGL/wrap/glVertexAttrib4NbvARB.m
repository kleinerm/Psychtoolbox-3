function glVertexAttrib4NbvARB( index, v )

% glVertexAttrib4NbvARB  Interface to OpenGL function glVertexAttrib4NbvARB
%
% usage:  glVertexAttrib4NbvARB( index, v )
%
% C function:  void glVertexAttrib4NbvARB(GLuint index, const GLbyte* v)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttrib4NbvARB', index, int8(v) );

return
