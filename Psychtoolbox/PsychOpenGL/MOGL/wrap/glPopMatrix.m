function glPopMatrix

% glPopMatrix  Interface to OpenGL function glPopMatrix
%
% usage:  glPopMatrix
%
% C function:  void glPopMatrix(void)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=0,
    error('invalid number of arguments');
end

moglcore( 'glPopMatrix' );

return
