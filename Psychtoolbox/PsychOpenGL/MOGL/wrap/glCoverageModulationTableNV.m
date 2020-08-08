function glCoverageModulationTableNV( n, v )

% glCoverageModulationTableNV  Interface to OpenGL function glCoverageModulationTableNV
%
% usage:  glCoverageModulationTableNV( n, v )
%
% C function:  void glCoverageModulationTableNV(GLsizei n, const GLfloat* v)

% 08-Aug-2020 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glCoverageModulationTableNV', n, single(v) );

return
