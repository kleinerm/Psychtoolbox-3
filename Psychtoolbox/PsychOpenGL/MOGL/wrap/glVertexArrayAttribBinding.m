function glVertexArrayAttribBinding( vaobj, attribindex, bindingindex )

% glVertexArrayAttribBinding  Interface to OpenGL function glVertexArrayAttribBinding
%
% usage:  glVertexArrayAttribBinding( vaobj, attribindex, bindingindex )
%
% C function:  void glVertexArrayAttribBinding(GLuint vaobj, GLuint attribindex, GLuint bindingindex)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glVertexArrayAttribBinding', vaobj, attribindex, bindingindex );

return
