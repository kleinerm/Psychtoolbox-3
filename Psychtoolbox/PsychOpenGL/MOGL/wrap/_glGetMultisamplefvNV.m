function val = glGetMultisamplefvNV( pname, index )

% glGetMultisamplefvNV  Interface to OpenGL function glGetMultisamplefvNV
%
% usage:  val = glGetMultisamplefvNV( pname, index )
%
% C function:  void glGetMultisamplefvNV(GLenum pname, GLuint index, GLfloat* val)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

val = single(0);

moglcore( 'glGetMultisamplefvNV', pname, index, val );

return
