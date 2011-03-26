function glEvalCoord2dv( u )

% glEvalCoord2dv  Interface to OpenGL function glEvalCoord2dv
%
% usage:  glEvalCoord2dv( u )
%
% C function:  void glEvalCoord2dv(const GLdouble* u)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glEvalCoord2dv', double(u) );

return
