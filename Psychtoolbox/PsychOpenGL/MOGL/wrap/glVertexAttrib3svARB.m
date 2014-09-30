function glVertexAttrib3svARB( index, v )

% glVertexAttrib3svARB  Interface to OpenGL function glVertexAttrib3svARB
%
% usage:  glVertexAttrib3svARB( index, v )
%
% C function:  void glVertexAttrib3svARB(GLuint index, const GLshort* v)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttrib3svARB', index, int16(v) );

return
