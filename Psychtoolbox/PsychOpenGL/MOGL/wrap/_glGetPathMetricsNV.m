function metrics = glGetPathMetricsNV( metricQueryMask, numPaths, pathNameType, paths, pathBase, stride )

% glGetPathMetricsNV  Interface to OpenGL function glGetPathMetricsNV
%
% usage:  metrics = glGetPathMetricsNV( metricQueryMask, numPaths, pathNameType, paths, pathBase, stride )
%
% C function:  void glGetPathMetricsNV(GLbitfield metricQueryMask, GLsizei numPaths, GLenum pathNameType, const void* paths, GLuint pathBase, GLsizei stride, GLfloat* metrics)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=6,
    error('invalid number of arguments');
end

metrics = single(0);

moglcore( 'glGetPathMetricsNV', metricQueryMask, numPaths, pathNameType, paths, pathBase, stride, metrics );

return
