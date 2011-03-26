function glRectsv( v1, v2 )

% glRectsv  Interface to OpenGL function glRectsv
%
% usage:  glRectsv( v1, v2 )
%
% C function:  void glRectsv(const GLshort* v1, const GLshort* v2)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glRectsv', int16(v1), int16(v2) );

return
