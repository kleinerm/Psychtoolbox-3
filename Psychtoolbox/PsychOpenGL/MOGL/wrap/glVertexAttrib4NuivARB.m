function glVertexAttrib4NuivARB( index, v )

% glVertexAttrib4NuivARB  Interface to OpenGL function glVertexAttrib4NuivARB
%
% usage:  glVertexAttrib4NuivARB( index, v )
%
% C function:  void glVertexAttrib4NuivARB(GLuint index, const GLuint* v)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttrib4NuivARB', index, uint32(v) );

return
