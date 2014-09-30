function glVertexAttrib2svARB( index, v )

% glVertexAttrib2svARB  Interface to OpenGL function glVertexAttrib2svARB
%
% usage:  glVertexAttrib2svARB( index, v )
%
% C function:  void glVertexAttrib2svARB(GLuint index, const GLshort* v)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttrib2svARB', index, int16(v) );

return
