function param = glGetTexBumpParameterfvATI( pname )

% glGetTexBumpParameterfvATI  Interface to OpenGL function glGetTexBumpParameterfvATI
%
% usage:  param = glGetTexBumpParameterfvATI( pname )
%
% C function:  void glGetTexBumpParameterfvATI(GLenum pname, GLfloat* param)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=1,
    error('invalid number of arguments');
end

param = single(0);

moglcore( 'glGetTexBumpParameterfvATI', pname, param );

return
