function [ data, bytesWritten ] = glGetPerfMonitorCounterDataAMD( monitor, pname, dataSize )

% glGetPerfMonitorCounterDataAMD  Interface to OpenGL function glGetPerfMonitorCounterDataAMD
%
% usage:  [ data, bytesWritten ] = glGetPerfMonitorCounterDataAMD( monitor, pname, dataSize )
%
% C function:  void glGetPerfMonitorCounterDataAMD(GLuint monitor, GLenum pname, GLsizei dataSize, GLuint* data, GLint* bytesWritten)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=3,
    error('invalid number of arguments');
end

data = uint32(0);
bytesWritten = int32(0);

moglcore( 'glGetPerfMonitorCounterDataAMD', monitor, pname, dataSize, data, bytesWritten );

return
