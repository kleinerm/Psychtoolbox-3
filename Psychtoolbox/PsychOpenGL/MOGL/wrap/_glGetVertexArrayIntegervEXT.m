function param = glGetVertexArrayIntegervEXT( vaobj, pname )

% glGetVertexArrayIntegervEXT  Interface to OpenGL function glGetVertexArrayIntegervEXT
%
% usage:  param = glGetVertexArrayIntegervEXT( vaobj, pname )
%
% C function:  void glGetVertexArrayIntegervEXT(GLuint vaobj, GLenum pname, GLint* param)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

param = int32(0);

moglcore( 'glGetVertexArrayIntegervEXT', vaobj, pname, param );

return
