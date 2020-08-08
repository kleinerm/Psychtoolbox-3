function glCommandListSegmentsNV( list, segments )

% glCommandListSegmentsNV  Interface to OpenGL function glCommandListSegmentsNV
%
% usage:  glCommandListSegmentsNV( list, segments )
%
% C function:  void glCommandListSegmentsNV(GLuint list, GLuint segments)

% 08-Aug-2020 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glCommandListSegmentsNV', list, segments );

return
