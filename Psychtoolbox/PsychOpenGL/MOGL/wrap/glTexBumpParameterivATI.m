function glTexBumpParameterivATI( pname, param )

% glTexBumpParameterivATI  Interface to OpenGL function glTexBumpParameterivATI
%
% usage:  glTexBumpParameterivATI( pname, param )
%
% C function:  void glTexBumpParameterivATI(GLenum pname, const GLint* param)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glTexBumpParameterivATI', pname, int32(param) );

return
