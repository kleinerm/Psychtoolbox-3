function [ length, counterString ] = glGetPerfMonitorCounterStringAMD( group, counter, bufSize )

% glGetPerfMonitorCounterStringAMD  Interface to OpenGL function glGetPerfMonitorCounterStringAMD
%
% usage:  [ length, counterString ] = glGetPerfMonitorCounterStringAMD( group, counter, bufSize )
%
% C function:  void glGetPerfMonitorCounterStringAMD(GLuint group, GLuint counter, GLsizei bufSize, GLsizei* length, GLchar* counterString)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=3,
    error('invalid number of arguments');
end

length = int32(0);
counterString = uint8(0);

moglcore( 'glGetPerfMonitorCounterStringAMD', group, counter, bufSize, length, counterString );

return
