function glEndPerfMonitorAMD( monitor )

% glEndPerfMonitorAMD  Interface to OpenGL function glEndPerfMonitorAMD
%
% usage:  glEndPerfMonitorAMD( monitor )
%
% C function:  void glEndPerfMonitorAMD(GLuint monitor)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glEndPerfMonitorAMD', monitor );

return
