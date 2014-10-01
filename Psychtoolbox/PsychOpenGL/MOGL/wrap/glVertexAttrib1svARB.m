function glVertexAttrib1svARB( index, v )

% glVertexAttrib1svARB  Interface to OpenGL function glVertexAttrib1svARB
%
% usage:  glVertexAttrib1svARB( index, v )
%
% C function:  void glVertexAttrib1svARB(GLuint index, const GLshort* v)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttrib1svARB', index, int16(v) );

return
