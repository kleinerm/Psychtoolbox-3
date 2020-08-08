function glCallCommandListNV( list )

% glCallCommandListNV  Interface to OpenGL function glCallCommandListNV
%
% usage:  glCallCommandListNV( list )
%
% C function:  void glCallCommandListNV(GLuint list)

% 08-Aug-2020 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glCallCommandListNV', list );

return
