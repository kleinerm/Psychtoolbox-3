function vec = Struct2Vect(struc,fieldnm)
% vec = Struct2Vect(struc,fieldnm)
% Traverses array of structs STRUC and returns data from all fields FIELDNM
% in vector VEC.
% Returns array vector if field contains numeric scalars, cell vector
% otherwise.
% Example: for field data.field:
% 
%   data(1).field = 23;
%   data(2).field = 56;
%   vec = struct2vec(data,'field')
%   vec = [23 56];
%
% DN 2007
% DN 2008-07-30 Fixed handling of numeric vectors

if isnumeric(struc(1).(fieldnm)) && isscalar(struc(1).(fieldnm))
    vec = [struc.(fieldnm)];
else % not numeric scalar, make cellarray
    vec = {struc.(fieldnm)};
end