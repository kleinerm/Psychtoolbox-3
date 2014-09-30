function glDisableVertexArrayAttribEXT( vaobj, index )

% glDisableVertexArrayAttribEXT  Interface to OpenGL function glDisableVertexArrayAttribEXT
%
% usage:  glDisableVertexArrayAttribEXT( vaobj, index )
%
% C function:  void glDisableVertexArrayAttribEXT(GLuint vaobj, GLuint index)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glDisableVertexArrayAttribEXT', vaobj, index );

return
