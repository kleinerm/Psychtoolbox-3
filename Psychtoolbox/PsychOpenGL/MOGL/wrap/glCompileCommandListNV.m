function glCompileCommandListNV( list )

% glCompileCommandListNV  Interface to OpenGL function glCompileCommandListNV
%
% usage:  glCompileCommandListNV( list )
%
% C function:  void glCompileCommandListNV(GLuint list)

% 08-Aug-2020 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glCompileCommandListNV', list );

return
