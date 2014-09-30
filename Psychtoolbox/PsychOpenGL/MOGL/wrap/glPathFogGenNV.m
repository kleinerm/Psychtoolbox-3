function glPathFogGenNV( genMode )

% glPathFogGenNV  Interface to OpenGL function glPathFogGenNV
%
% usage:  glPathFogGenNV( genMode )
%
% C function:  void glPathFogGenNV(GLenum genMode)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glPathFogGenNV', genMode );

return
