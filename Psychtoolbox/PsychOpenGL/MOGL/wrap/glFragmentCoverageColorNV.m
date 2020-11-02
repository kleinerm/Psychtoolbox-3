function glFragmentCoverageColorNV( color )

% glFragmentCoverageColorNV  Interface to OpenGL function glFragmentCoverageColorNV
%
% usage:  glFragmentCoverageColorNV( color )
%
% C function:  void glFragmentCoverageColorNV(GLuint color)

% 08-Aug-2020 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glFragmentCoverageColorNV', color );

return
