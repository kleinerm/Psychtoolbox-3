function glVertexAttribDivisor( index, divisor )

% glVertexAttribDivisor  Interface to OpenGL function glVertexAttribDivisor
%
% usage:  glVertexAttribDivisor( index, divisor )
%
% C function:  void glVertexAttribDivisor(GLuint index, GLuint divisor)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttribDivisor', index, divisor );

return
