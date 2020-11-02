function v = glGetCoverageModulationTableNV( bufsize )

% glGetCoverageModulationTableNV  Interface to OpenGL function glGetCoverageModulationTableNV
%
% usage:  v = glGetCoverageModulationTableNV( bufsize )
%
% C function:  void glGetCoverageModulationTableNV(GLsizei bufsize, GLfloat* v)

% 08-Aug-2020 -- created (generated automatically from header files)

% ---allocate---

if nargin~=1,
    error('invalid number of arguments');
end

v = single(0);

moglcore( 'glGetCoverageModulationTableNV', bufsize, v );

return
