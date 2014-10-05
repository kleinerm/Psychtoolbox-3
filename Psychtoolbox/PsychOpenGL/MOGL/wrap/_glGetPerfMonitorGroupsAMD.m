function [ numGroups, groups ] = glGetPerfMonitorGroupsAMD( groupsSize )

% glGetPerfMonitorGroupsAMD  Interface to OpenGL function glGetPerfMonitorGroupsAMD
%
% usage:  [ numGroups, groups ] = glGetPerfMonitorGroupsAMD( groupsSize )
%
% C function:  void glGetPerfMonitorGroupsAMD(GLint* numGroups, GLsizei groupsSize, GLuint* groups)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=1,
    error('invalid number of arguments');
end

numGroups = int32(0);
groups = uint32(0);

moglcore( 'glGetPerfMonitorGroupsAMD', numGroups, groupsSize, groups );

return
