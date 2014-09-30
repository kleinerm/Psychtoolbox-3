function glDisableVertexArrayEXT( vaobj, array )

% glDisableVertexArrayEXT  Interface to OpenGL function glDisableVertexArrayEXT
%
% usage:  glDisableVertexArrayEXT( vaobj, array )
%
% C function:  void glDisableVertexArrayEXT(GLuint vaobj, GLenum array)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glDisableVertexArrayEXT', vaobj, array );

return
