function metrics = glGetPathMetricRangeNV( metricQueryMask, firstPathName, numPaths, stride )

% glGetPathMetricRangeNV  Interface to OpenGL function glGetPathMetricRangeNV
%
% usage:  metrics = glGetPathMetricRangeNV( metricQueryMask, firstPathName, numPaths, stride )
%
% C function:  void glGetPathMetricRangeNV(GLbitfield metricQueryMask, GLuint firstPathName, GLsizei numPaths, GLsizei stride, GLfloat* metrics)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=4,
    error('invalid number of arguments');
end

metrics = single(0);

moglcore( 'glGetPathMetricRangeNV', metricQueryMask, firstPathName, numPaths, stride, metrics );

return
