function glCopyPathNV( resultPath, srcPath )

% glCopyPathNV  Interface to OpenGL function glCopyPathNV
%
% usage:  glCopyPathNV( resultPath, srcPath )
%
% C function:  void glCopyPathNV(GLuint resultPath, GLuint srcPath)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glCopyPathNV', resultPath, srcPath );

return
