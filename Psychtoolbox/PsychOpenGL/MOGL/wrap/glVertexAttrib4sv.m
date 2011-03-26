function glVertexAttrib4sv( index, v )

% glVertexAttrib4sv  Interface to OpenGL function glVertexAttrib4sv
%
% usage:  glVertexAttrib4sv( index, v )
%
% C function:  void glVertexAttrib4sv(GLuint index, const GLshort* v)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttrib4sv', index, int16(v) );

return
