function [loglik,m,d] = FitCumGauss_MES(p,r,mset,dset,guessrate)
% Fits a cumulative Gaussian to a set of probe values and subject responses
% 
% [loglik,m,d] = FitCumGauss_MES(p,r,mset,dset,guessrate)
% Computes likelihood of each combination of PSEs mset and slopes dset
% given a set of probe values and responses using a cumulative Gaussian.
%
% For ease of thinking about it you can view responses as either 0 or 1,
% though in practice anything larger than 0 is treated as 1 and anything
% lower than 0, including 0, is treated as 0. A 1, -1 response scheme as
% input is thus no problem.
%
% Optinally fits with a guess rate, default 0. If a guess rate is set, the
% cumulative Gaussian levels off at guessrate/2 and 1-guessrate/2.

% Copyright (c) 2011 by DC Niehorster and JA Saunders

if nargin < 5
    guessrate = 0;
end

g0 = guessrate/2;
g1 = 1 - guessrate;
g2 = 1 - guessrate/2;

[msamp,dsamp]   = meshgrid(mset,dset);
sz              = size(msamp);
msamp           = msamp(:).';
dsamp           = dsamp(:).';

% the below are equivalent ways of computing the fit, lowest is fastest but
% most obtuse, the others are provided for documentation purposes
if 0
    
    % slow way with a loop,
    % but less memory intensive
    
    lik = zeros(size(msamp));
    for ksamp = 1:length(msamp)
        for ktrial = 1:length(r)
            if(r(ktrial) > 0)
                currlik = log(g0 + g1*normcdf( (p(ktrial)-msamp(ksamp))/dsamp(ksamp)));
                % reduces to:
                % currlik = log(normcdf( (p(ktrial)-msamp(ksamp))/dsamp(ksamp)));
                % when guessrate is 0
            else
                currlik = log(g2 - g1*normcdf( (p(ktrial)-msamp(ksamp))/dsamp(ksamp)));
                % currlik = log(1.0 - normcdf( (p(ktrial)-msamp(ksamp))/dsamp(ksamp)));
            end
            lik(ksamp) = lik(ksamp) + currlik;
        end
    end
    
elseif 0 % choose this one if the below get you into lack of memory trouble
    % faster way by blocking, but still looping over trials
    lik = zeros(size(msamp));
    for ktrial = 1:length(r)
        if(r(ktrial) > 0)
            currlik = log(g0 + g1*normcdf( (p(ktrial)-msamp)./dsamp) );
            % reduces to:
            % currlik = log(normcdf( (p(ktrial)-msamp)./dsamp));
            % when guessrate is 0
        else
            currlik = log(g2 - g1*normcdf( (p(ktrial)-msamp)./dsamp) );
            % currlik = log(1.0 - normcdf( (p(ktrial)-msamp)./dsamp));
        end
        lik     = lik + currlik;
    end
    
elseif 0
    % faster way by blocking,
    % but more memory intensive
    rmat        = repmat(r(:)>0,[1,length(msamp)]);
    mmat        = repmat(msamp,[length(r),1]);
    dmat        = repmat(dsamp,[length(r),1]);
    pmat        = repmat(p(:),[1,length(msamp)]);
    
    temp        = g2 - g1*normcdf( (pmat-mmat)./dmat);
    ind         = find(rmat(:));
    temp(ind)   = 1 - temp(ind);
    loglik      = sum(log(temp),1);
    
else
    % slightly faster and less memory intensive way than previous, using
    % bsxfun (very slightly actually, but hey)
    temp        = g2 - g1*normcdf( bsxfun(@rdivide,bsxfun(@minus,p(:),msamp),dsamp));
    rmat        = repmat(r(:)>0,[1,length(msamp)]);
    ind         = find(rmat(:));
    temp(ind)   = 1 - temp(ind);
    loglik      = sum(log(temp),1);
end

kmin    = find(loglik == max(loglik));
m       = mean(msamp(kmin));
d       = mean(dsamp(kmin));

loglik  = reshape(loglik,sz);
%imagesc(exp(0.5*loglik))