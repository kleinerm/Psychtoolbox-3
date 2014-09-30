function glEnableVertexArrayAttribEXT( vaobj, index )

% glEnableVertexArrayAttribEXT  Interface to OpenGL function glEnableVertexArrayAttribEXT
%
% usage:  glEnableVertexArrayAttribEXT( vaobj, index )
%
% C function:  void glEnableVertexArrayAttribEXT(GLuint vaobj, GLuint index)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glEnableVertexArrayAttribEXT', vaobj, index );

return
