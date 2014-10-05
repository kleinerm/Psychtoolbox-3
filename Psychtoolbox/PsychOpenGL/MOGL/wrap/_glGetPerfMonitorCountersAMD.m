function [ numCounters, maxActiveCounters, counters ] = glGetPerfMonitorCountersAMD( group, counterSize )

% glGetPerfMonitorCountersAMD  Interface to OpenGL function glGetPerfMonitorCountersAMD
%
% usage:  [ numCounters, maxActiveCounters, counters ] = glGetPerfMonitorCountersAMD( group, counterSize )
%
% C function:  void glGetPerfMonitorCountersAMD(GLuint group, GLint* numCounters, GLint* maxActiveCounters, GLsizei counterSize, GLuint* counters)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

numCounters = int32(0);
maxActiveCounters = int32(0);
counters = uint32(0);

moglcore( 'glGetPerfMonitorCountersAMD', group, numCounters, maxActiveCounters, counterSize, counters );

return
