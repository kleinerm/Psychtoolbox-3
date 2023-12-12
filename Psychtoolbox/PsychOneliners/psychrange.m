function r = psychrange(varargin)
% r = psychrange(X [, dims]) - Drop in replacement for range().
%
% See Matlab online docs, or Octave help for range().
%
% Matlab does not have range() outside the statistics toolbox, and we do
% not want to enforce installation of it for the few use-cases we have
% internally for range(), so provide this drop-in / fallback inplementation.
%
% Octave would not benefit from this, as range is part of statistics package,
% but so are min and max, so if range isn't there, the min and max functions
% needed here won't be either.
%

% History
% 01-Feb-2021   mk  Written.

    % range() exists?
    if ismember(exist('range'), [2,3])
        % Use it:
        r = range(varargin{:});
    else
        % Implement trivial fallback implementation:
        if nargin < 1 || nargin > 2
            error('Invalid number of arguments to fallback psychrange() provided. Must be one or two.');
        end

        if nargin == 1
            r = max(varargin{1}) - min(varargin{1});
        else
            r = max(varargin{1}, [], varargin{2}) - min(varargin{1}, [], varargin{2});
        end
    end
end
