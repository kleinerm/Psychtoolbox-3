function [vec] = interleave(varargin)
% [vec] = interleave(varargin)
% interleaves any number of vectors and scalars
% input is taken from consequetive elements of each input variable until a
% variable runs out.
% Output will be a columnvector.
% Output will be cell if any of the inputs is cell.
% Output will be char if none of the input is cell and any of the inputs is
% char
% 
% a   = [A A A], b= [B B], c= [C C C C], d=D
% out = interleave(a,b,c,d)
% out = [A B C D A B C A C C];
%
% DN 06-11-2007
% DN 23-01-2008 updated to support scalars and conversion to char
% DN 28-04-2008 simplified input checking
% DN 28-05-2008 bugfix mixing numeric and char

cellfun(@(x)psychassert(isvector(x) || isscalar(x),'not all inputs are vectors or scalars'),varargin);
cellfun(@(x)psychassert(isnumeric(x) || iscell(x) || ischar(x),'not all inputs are numeric, cell or char'),varargin);

% check what input we have
qchar = any(cellfun(@ischar,varargin));
qcell = any(cellfun(@iscell,varargin));
qnum  = cellfun(@isnumeric,varargin);

if any(qnum) && qchar && ~qcell
    % convert all numeric inputs to char
    inds = find(qnum);
    for p=1:length(inds)
        temp = num2cell(varargin{inds(p)});
        varargin{inds(p)} = cellfun(@num2str,temp,'UniformOutput',false);
    end
end

for p=1:numel(varargin)
    r(p,1:numel(varargin{p})) = num2cell(varargin{p});
end
vec=[r{:}];

if any(qnum) && qchar && ~qcell
    % last step
    vec=[vec{:}];
end
