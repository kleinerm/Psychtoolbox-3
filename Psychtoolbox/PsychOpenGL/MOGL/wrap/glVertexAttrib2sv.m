function glVertexAttrib2sv( index, v )

% glVertexAttrib2sv  Interface to OpenGL function glVertexAttrib2sv
%
% usage:  glVertexAttrib2sv( index, v )
%
% C function:  void glVertexAttrib2sv(GLuint index, const GLshort* v)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttrib2sv', index, int16(v) );

return
