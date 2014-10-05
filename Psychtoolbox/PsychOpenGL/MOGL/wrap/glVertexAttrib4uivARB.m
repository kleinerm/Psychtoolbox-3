function glVertexAttrib4uivARB( index, v )

% glVertexAttrib4uivARB  Interface to OpenGL function glVertexAttrib4uivARB
%
% usage:  glVertexAttrib4uivARB( index, v )
%
% C function:  void glVertexAttrib4uivARB(GLuint index, const GLuint* v)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttrib4uivARB', index, uint32(v) );

return
