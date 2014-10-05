function glBeginPerfMonitorAMD( monitor )

% glBeginPerfMonitorAMD  Interface to OpenGL function glBeginPerfMonitorAMD
%
% usage:  glBeginPerfMonitorAMD( monitor )
%
% C function:  void glBeginPerfMonitorAMD(GLuint monitor)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glBeginPerfMonitorAMD', monitor );

return
