function glDispatchComputeGroupSizeARB( num_groups_x, num_groups_y, num_groups_z, group_size_x, group_size_y, group_size_z )

% glDispatchComputeGroupSizeARB  Interface to OpenGL function glDispatchComputeGroupSizeARB
%
% usage:  glDispatchComputeGroupSizeARB( num_groups_x, num_groups_y, num_groups_z, group_size_x, group_size_y, group_size_z )
%
% C function:  void glDispatchComputeGroupSizeARB(GLuint num_groups_x, GLuint num_groups_y, GLuint num_groups_z, GLuint group_size_x, GLuint group_size_y, GLuint group_size_z)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=6,
    error('invalid number of arguments');
end

moglcore( 'glDispatchComputeGroupSizeARB', num_groups_x, num_groups_y, num_groups_z, group_size_x, group_size_y, group_size_z );

return
