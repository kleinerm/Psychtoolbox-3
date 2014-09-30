function glResetHistogramEXT( target )

% glResetHistogramEXT  Interface to OpenGL function glResetHistogramEXT
%
% usage:  glResetHistogramEXT( target )
%
% C function:  void glResetHistogramEXT(GLenum target)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glResetHistogramEXT', target );

return
