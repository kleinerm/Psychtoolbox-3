function data = glGetPerfMonitorCounterInfoAMD( group, counter, pname )

% glGetPerfMonitorCounterInfoAMD  Interface to OpenGL function glGetPerfMonitorCounterInfoAMD
%
% usage:  data = glGetPerfMonitorCounterInfoAMD( group, counter, pname )
%
% C function:  void glGetPerfMonitorCounterInfoAMD(GLuint group, GLuint counter, GLenum pname, void* data)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=3,
    error('invalid number of arguments');
end

data = (0);

moglcore( 'glGetPerfMonitorCounterInfoAMD', group, counter, pname, data );

return
