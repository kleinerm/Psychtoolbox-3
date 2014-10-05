function glMatrixPopEXT( mode )

% glMatrixPopEXT  Interface to OpenGL function glMatrixPopEXT
%
% usage:  glMatrixPopEXT( mode )
%
% C function:  void glMatrixPopEXT(GLenum mode)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glMatrixPopEXT', mode );

return
