function alDistanceModel( distanceModel )

% alDistanceModel  Interface to OpenAL function alDistanceModel
%
% usage:  alDistanceModel( distanceModel )
%
% C function:  void alDistanceModel(ALenum distanceModel)

% 28-Mar-2011 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moalcore( 'alDistanceModel', distanceModel );

return
