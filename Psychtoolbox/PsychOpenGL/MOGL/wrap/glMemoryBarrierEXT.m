function glMemoryBarrierEXT( barriers )

% glMemoryBarrierEXT  Interface to OpenGL function glMemoryBarrierEXT
%
% usage:  glMemoryBarrierEXT( barriers )
%
% C function:  void glMemoryBarrierEXT(GLbitfield barriers)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glMemoryBarrierEXT', barriers );

return
