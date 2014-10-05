function params = glGetObjectBufferfvATI( buffer, pname )

% glGetObjectBufferfvATI  Interface to OpenGL function glGetObjectBufferfvATI
%
% usage:  params = glGetObjectBufferfvATI( buffer, pname )
%
% C function:  void glGetObjectBufferfvATI(GLuint buffer, GLenum pname, GLfloat* params)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

params = single(0);

moglcore( 'glGetObjectBufferfvATI', buffer, pname, params );

return
