function glVertexAttrib4ivARB( index, v )

% glVertexAttrib4ivARB  Interface to OpenGL function glVertexAttrib4ivARB
%
% usage:  glVertexAttrib4ivARB( index, v )
%
% C function:  void glVertexAttrib4ivARB(GLuint index, const GLint* v)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttrib4ivARB', index, int32(v) );

return
