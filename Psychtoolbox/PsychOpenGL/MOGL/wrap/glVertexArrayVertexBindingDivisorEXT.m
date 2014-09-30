function glVertexArrayVertexBindingDivisorEXT( vaobj, bindingindex, divisor )

% glVertexArrayVertexBindingDivisorEXT  Interface to OpenGL function glVertexArrayVertexBindingDivisorEXT
%
% usage:  glVertexArrayVertexBindingDivisorEXT( vaobj, bindingindex, divisor )
%
% C function:  void glVertexArrayVertexBindingDivisorEXT(GLuint vaobj, GLuint bindingindex, GLuint divisor)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glVertexArrayVertexBindingDivisorEXT', vaobj, bindingindex, divisor );

return
