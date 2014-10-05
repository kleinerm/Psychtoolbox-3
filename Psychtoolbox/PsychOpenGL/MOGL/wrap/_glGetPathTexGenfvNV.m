function value = glGetPathTexGenfvNV( texCoordSet, pname )

% glGetPathTexGenfvNV  Interface to OpenGL function glGetPathTexGenfvNV
%
% usage:  value = glGetPathTexGenfvNV( texCoordSet, pname )
%
% C function:  void glGetPathTexGenfvNV(GLenum texCoordSet, GLenum pname, GLfloat* value)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

value = single(0);

moglcore( 'glGetPathTexGenfvNV', texCoordSet, pname, value );

return
