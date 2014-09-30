function glEnableVertexArrayEXT( vaobj, array )

% glEnableVertexArrayEXT  Interface to OpenGL function glEnableVertexArrayEXT
%
% usage:  glEnableVertexArrayEXT( vaobj, array )
%
% C function:  void glEnableVertexArrayEXT(GLuint vaobj, GLenum array)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glEnableVertexArrayEXT', vaobj, array );

return
