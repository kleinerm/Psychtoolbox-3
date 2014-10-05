function glEnableVertexArrayAttrib( vaobj, index )

% glEnableVertexArrayAttrib  Interface to OpenGL function glEnableVertexArrayAttrib
%
% usage:  glEnableVertexArrayAttrib( vaobj, index )
%
% C function:  void glEnableVertexArrayAttrib(GLuint vaobj, GLuint index)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glEnableVertexArrayAttrib', vaobj, index );

return
