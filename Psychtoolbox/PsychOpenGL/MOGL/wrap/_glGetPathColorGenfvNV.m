function value = glGetPathColorGenfvNV( color, pname )

% glGetPathColorGenfvNV  Interface to OpenGL function glGetPathColorGenfvNV
%
% usage:  value = glGetPathColorGenfvNV( color, pname )
%
% C function:  void glGetPathColorGenfvNV(GLenum color, GLenum pname, GLfloat* value)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

value = single(0);

moglcore( 'glGetPathColorGenfvNV', color, pname, value );

return
