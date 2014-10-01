function params = glGetArrayObjectfvATI( array, pname )

% glGetArrayObjectfvATI  Interface to OpenGL function glGetArrayObjectfvATI
%
% usage:  params = glGetArrayObjectfvATI( array, pname )
%
% C function:  void glGetArrayObjectfvATI(GLenum array, GLenum pname, GLfloat* params)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

params = single(0);

moglcore( 'glGetArrayObjectfvATI', array, pname, params );

return
