function glPopMatrix

% glPopMatrix  Interface to OpenGL function glPopMatrix
%
% usage:  glPopMatrix
%
% C function:  void glPopMatrix(void)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=0,
    error('invalid number of arguments');
end

moglcore( 'glPopMatrix' );

return
