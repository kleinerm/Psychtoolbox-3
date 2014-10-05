function monitors = glGenPerfMonitorsAMD( n )

% glGenPerfMonitorsAMD  Interface to OpenGL function glGenPerfMonitorsAMD
%
% usage:  monitors = glGenPerfMonitorsAMD( n )
%
% C function:  void glGenPerfMonitorsAMD(GLsizei n, GLuint* monitors)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=1,
    error('invalid number of arguments');
end

monitors = uint32(0);

moglcore( 'glGenPerfMonitorsAMD', n, monitors );

return
