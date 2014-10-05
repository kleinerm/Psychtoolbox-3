function glDisableVertexArrayAttrib( vaobj, index )

% glDisableVertexArrayAttrib  Interface to OpenGL function glDisableVertexArrayAttrib
%
% usage:  glDisableVertexArrayAttrib( vaobj, index )
%
% C function:  void glDisableVertexArrayAttrib(GLuint vaobj, GLuint index)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glDisableVertexArrayAttrib', vaobj, index );

return
