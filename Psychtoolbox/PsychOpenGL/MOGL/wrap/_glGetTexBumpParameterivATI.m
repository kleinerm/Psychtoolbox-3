function param = glGetTexBumpParameterivATI( pname )

% glGetTexBumpParameterivATI  Interface to OpenGL function glGetTexBumpParameterivATI
%
% usage:  param = glGetTexBumpParameterivATI( pname )
%
% C function:  void glGetTexBumpParameterivATI(GLenum pname, GLint* param)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=1,
    error('invalid number of arguments');
end

param = int32(0);

moglcore( 'glGetTexBumpParameterivATI', pname, param );

return
