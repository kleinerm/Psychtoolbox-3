function glVertexArrayVertexAttribBindingEXT( vaobj, attribindex, bindingindex )

% glVertexArrayVertexAttribBindingEXT  Interface to OpenGL function glVertexArrayVertexAttribBindingEXT
%
% usage:  glVertexArrayVertexAttribBindingEXT( vaobj, attribindex, bindingindex )
%
% C function:  void glVertexArrayVertexAttribBindingEXT(GLuint vaobj, GLuint attribindex, GLuint bindingindex)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glVertexArrayVertexAttribBindingEXT', vaobj, attribindex, bindingindex );

return
