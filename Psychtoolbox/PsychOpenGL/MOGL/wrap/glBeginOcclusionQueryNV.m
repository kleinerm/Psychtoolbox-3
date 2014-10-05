function glBeginOcclusionQueryNV( id )

% glBeginOcclusionQueryNV  Interface to OpenGL function glBeginOcclusionQueryNV
%
% usage:  glBeginOcclusionQueryNV( id )
%
% C function:  void glBeginOcclusionQueryNV(GLuint id)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glBeginOcclusionQueryNV', id );

return
