function params = glGetArrayObjectivATI( array, pname )

% glGetArrayObjectivATI  Interface to OpenGL function glGetArrayObjectivATI
%
% usage:  params = glGetArrayObjectivATI( array, pname )
%
% C function:  void glGetArrayObjectivATI(GLenum array, GLenum pname, GLint* params)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

params = int32(0);

moglcore( 'glGetArrayObjectivATI', array, pname, params );

return
