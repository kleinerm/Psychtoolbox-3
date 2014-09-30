function glVertexAttrib4NivARB( index, v )

% glVertexAttrib4NivARB  Interface to OpenGL function glVertexAttrib4NivARB
%
% usage:  glVertexAttrib4NivARB( index, v )
%
% C function:  void glVertexAttrib4NivARB(GLuint index, const GLint* v)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttrib4NivARB', index, int32(v) );

return
