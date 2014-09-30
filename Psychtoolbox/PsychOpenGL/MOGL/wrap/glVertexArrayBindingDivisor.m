function glVertexArrayBindingDivisor( vaobj, bindingindex, divisor )

% glVertexArrayBindingDivisor  Interface to OpenGL function glVertexArrayBindingDivisor
%
% usage:  glVertexArrayBindingDivisor( vaobj, bindingindex, divisor )
%
% C function:  void glVertexArrayBindingDivisor(GLuint vaobj, GLuint bindingindex, GLuint divisor)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glVertexArrayBindingDivisor', vaobj, bindingindex, divisor );

return
