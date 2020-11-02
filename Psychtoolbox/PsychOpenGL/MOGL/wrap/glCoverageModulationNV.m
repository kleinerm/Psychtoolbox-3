function glCoverageModulationNV( components )

% glCoverageModulationNV  Interface to OpenGL function glCoverageModulationNV
%
% usage:  glCoverageModulationNV( components )
%
% C function:  void glCoverageModulationNV(GLenum components)

% 08-Aug-2020 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glCoverageModulationNV', components );

return
