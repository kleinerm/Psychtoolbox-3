function glEvalMapsNV( target, mode )

% glEvalMapsNV  Interface to OpenGL function glEvalMapsNV
%
% usage:  glEvalMapsNV( target, mode )
%
% C function:  void glEvalMapsNV(GLenum target, GLenum mode)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glEvalMapsNV', target, mode );

return
