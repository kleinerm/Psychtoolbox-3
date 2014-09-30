function glVertexArrayVertexAttribDivisorEXT( vaobj, index, divisor )

% glVertexArrayVertexAttribDivisorEXT  Interface to OpenGL function glVertexArrayVertexAttribDivisorEXT
%
% usage:  glVertexArrayVertexAttribDivisorEXT( vaobj, index, divisor )
%
% C function:  void glVertexArrayVertexAttribDivisorEXT(GLuint vaobj, GLuint index, GLuint divisor)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glVertexArrayVertexAttribDivisorEXT', vaobj, index, divisor );

return
