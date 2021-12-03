function [uEst,varEst] = FitCumNormYN(inputs,nYes,nNo)
% [uEst,varEst] = FitCumNormYN(inputs,nYes,nNo)
%
% Fits a cumulative normal function to the passed yes-no data.
% Requires the optimization toolbox.
%
% INPUTS:
%   inputs:   Input levels
%   nYes:     Number of yes responses at the corresponding input level
%   nNo:      Number of no responses at the corresponding input level
% OUTPUTS:
%   uEst
%   varEst
%
% See also: FitWeibTAFC, FitFitWeibYN, FitWeibAlphTAFC, FitLogitYN
%
% 9/22/93   jms  Created from FitWeibullYN.
% 2/8/97    dhb  Cleaned up and added some comments.
%                Check that optimization toolbox is present.
% 10/4/00   dhb  Fixed bugs along lines suggested by Keith Schneider.
%                Case of uInitial = 0 wasn't handled properly, and
%                variance search limits were set based on mean.
% 3/4/05    dhb  Conditionals for optimization toolbox version.

if IsOctave
    v = version;
    if str2num(v(1)) < 6
        error('For use with Octave, you need at least Octave version 6.');
    end

    try
        % Try loading the optim package with the optimization functions:
        pkg load optim;
    catch
        error('For use with Octave, you must install the ''optim'' package from Octave Forge. See ''help pkg''.');
    end

    % Got optim package loaded. Does it support fmincon()?
    if ~exist('fmincon')
        error('For use with Octave, you need at least version 1.6.0 of the ''optim'' package from Octave Forge.');
    end
end

% Set up an initial guess
uInitial = mean(inputs);
varInitial = std(inputs)^2;

% Stuff guess into a vector
x0(1) = uInitial;
x0(2) = varInitial;
vlb = [-1e10; 1e-10*varInitial];
vub = [1e10; 1e10*varInitial];

% Check for needed optimization toolbox, and version.
if (exist('fmincon') == 2) && (IsOctave || exist('getIpOptions'))
    options = optimset;
    options = optimset(options,'Display','off');
    if ~IsOctave
        options = optimset(options,'LargeScale','off');
    end
    x1 = fmincon(@CumNormYNFitFun,x0,[],[],[],[],vlb,vub,[],options);
    x = fmincon(@CumNormYNFitFun,x1,[],[],[],[],vlb,vub,[],options);
elseif (exist('constr') == 2)
    options = foptions;
    options(1) = 0;
    x1 = constr(@CumNormYNFitFun,x0,options,vlb,vub);
    x = constr(@CumNormYNFitFun,x1,options,vlb,vub);
else
    error('FitCumNormYN requires the optional Matlab Optimization Toolbox from Mathworks');
end

% Extract fit parameters parameters
uEst =   x(1);
varEst = x(2);

    % Nested target function to optimize:
    function [f,g] = CumNormYNFitFun(x)
        % [f,g] = CumNormYNFitFun(x)
        %
        % 9/22/93   jms  Created from FitWeibullYN.

        u    = x(1);
        theVar  = x(2);

        pYes = NormalCumulative(inputs,u,theVar);

        % Handle range problem, can't take log(0);
        tol = 1e-4;
        z_index = find(pYes == 0);
        if (~isempty(z_index))
          pYes(z_index) = tol*ones(length(z_index),1);
        end
        o_index = find(pYes == 1);
        if (~isempty(o_index))
          pYes(o_index) = (1-tol)*ones(length(o_index),1);
        end

        % Compute error
        tmp = nYes.*log(pYes) + nNo.*log(1 - pYes);
        f =  -sum(tmp);
        g = -1;
    end
end
