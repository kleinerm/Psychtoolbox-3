function [ length, groupString ] = glGetPerfMonitorGroupStringAMD( group, bufSize )

% glGetPerfMonitorGroupStringAMD  Interface to OpenGL function glGetPerfMonitorGroupStringAMD
%
% usage:  [ length, groupString ] = glGetPerfMonitorGroupStringAMD( group, bufSize )
%
% C function:  void glGetPerfMonitorGroupStringAMD(GLuint group, GLsizei bufSize, GLsizei* length, GLchar* groupString)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

length = int32(0);
groupString = uint8(0);

moglcore( 'glGetPerfMonitorGroupStringAMD', group, bufSize, length, groupString );

return
