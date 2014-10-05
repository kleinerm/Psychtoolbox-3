function monitors = glDeletePerfMonitorsAMD( n )

% glDeletePerfMonitorsAMD  Interface to OpenGL function glDeletePerfMonitorsAMD
%
% usage:  monitors = glDeletePerfMonitorsAMD( n )
%
% C function:  void glDeletePerfMonitorsAMD(GLsizei n, GLuint* monitors)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=1,
    error('invalid number of arguments');
end

monitors = uint32(0);

moglcore( 'glDeletePerfMonitorsAMD', n, monitors );

return
