function glVertexAttrib4ubvARB( index, v )

% glVertexAttrib4ubvARB  Interface to OpenGL function glVertexAttrib4ubvARB
%
% usage:  glVertexAttrib4ubvARB( index, v )
%
% C function:  void glVertexAttrib4ubvARB(GLuint index, const GLubyte* v)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttrib4ubvARB', index, uint8(v) );

return
