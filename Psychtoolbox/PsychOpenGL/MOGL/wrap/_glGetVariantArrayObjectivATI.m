function params = glGetVariantArrayObjectivATI( id, pname )

% glGetVariantArrayObjectivATI  Interface to OpenGL function glGetVariantArrayObjectivATI
%
% usage:  params = glGetVariantArrayObjectivATI( id, pname )
%
% C function:  void glGetVariantArrayObjectivATI(GLuint id, GLenum pname, GLint* params)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

params = int32(0);

moglcore( 'glGetVariantArrayObjectivATI', id, pname, params );

return
