function glVertexAttrib4bvARB( index, v )

% glVertexAttrib4bvARB  Interface to OpenGL function glVertexAttrib4bvARB
%
% usage:  glVertexAttrib4bvARB( index, v )
%
% C function:  void glVertexAttrib4bvARB(GLuint index, const GLbyte* v)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttrib4bvARB', index, int8(v) );

return
