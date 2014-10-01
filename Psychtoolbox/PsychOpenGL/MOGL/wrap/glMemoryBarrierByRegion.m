function glMemoryBarrierByRegion( barriers )

% glMemoryBarrierByRegion  Interface to OpenGL function glMemoryBarrierByRegion
%
% usage:  glMemoryBarrierByRegion( barriers )
%
% C function:  void glMemoryBarrierByRegion(GLbitfield barriers)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glMemoryBarrierByRegion', barriers );

return
