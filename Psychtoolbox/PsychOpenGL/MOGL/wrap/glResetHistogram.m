function glResetHistogram( target )

% glResetHistogram  Interface to OpenGL function glResetHistogram
%
% usage:  glResetHistogram( target )
%
% C function:  void glResetHistogram(GLenum target)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glResetHistogram', target );

return
