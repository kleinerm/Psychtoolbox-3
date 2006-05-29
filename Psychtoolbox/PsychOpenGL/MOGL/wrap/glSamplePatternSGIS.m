function glSamplePatternSGIS( arg1 )

% glSamplePatternSGIS  Interface to OpenGL function glSamplePatternSGIS
%
% usage:  glSamplePatternSGIS( arg1 )
%
% C function:  void glSamplePatternSGIS(GLenum)

% 30-May-2006 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glSamplePatternSGIS', arg1 );

return
