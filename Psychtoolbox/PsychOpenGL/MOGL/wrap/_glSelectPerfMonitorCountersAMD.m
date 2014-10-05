function counterList = glSelectPerfMonitorCountersAMD( monitor, enable, group, numCounters )

% glSelectPerfMonitorCountersAMD  Interface to OpenGL function glSelectPerfMonitorCountersAMD
%
% usage:  counterList = glSelectPerfMonitorCountersAMD( monitor, enable, group, numCounters )
%
% C function:  void glSelectPerfMonitorCountersAMD(GLuint monitor, GLboolean enable, GLuint group, GLint numCounters, GLuint* counterList)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=4,
    error('invalid number of arguments');
end

counterList = uint32(0);

moglcore( 'glSelectPerfMonitorCountersAMD', monitor, enable, group, numCounters, counterList );

return
