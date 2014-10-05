function glTexBumpParameterfvATI( pname, param )

% glTexBumpParameterfvATI  Interface to OpenGL function glTexBumpParameterfvATI
%
% usage:  glTexBumpParameterfvATI( pname, param )
%
% C function:  void glTexBumpParameterfvATI(GLenum pname, const GLfloat* param)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glTexBumpParameterfvATI', pname, single(param) );

return
