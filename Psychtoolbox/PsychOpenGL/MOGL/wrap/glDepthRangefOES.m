function glDepthRangefOES( n, f )

% glDepthRangefOES  Interface to OpenGL function glDepthRangefOES
%
% usage:  glDepthRangefOES( n, f )
%
% C function:  void glDepthRangefOES(GLclampf n, GLclampf f)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glDepthRangefOES', n, f );

return
