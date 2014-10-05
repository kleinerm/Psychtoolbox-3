function params = glGetVariantArrayObjectfvATI( id, pname )

% glGetVariantArrayObjectfvATI  Interface to OpenGL function glGetVariantArrayObjectfvATI
%
% usage:  params = glGetVariantArrayObjectfvATI( id, pname )
%
% C function:  void glGetVariantArrayObjectfvATI(GLuint id, GLenum pname, GLfloat* params)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

params = single(0);

moglcore( 'glGetVariantArrayObjectfvATI', id, pname, params );

return
