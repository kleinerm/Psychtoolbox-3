function spd = PR705measspd(S)
% PR705measspd - Get a spectral power distribution from the PR-705.
%
% Syntax:
% spd = PR705measspd([S])
%
% Input:
% S (1x3) - Desired wavelength sampling. Default: [380 5 81]
%
% Output:
% spd (Nx1) - The spectral power distribution (N = S(3) or 81 by default).
%
% 12/06/12   zlb   Wrote it loosely based on the PR670Toolbox.

% PR-705 measurement error codes
%   -5000: Weak Signal
%   -4999: Time underflow, level overflow
%   -4996: A2D overflow, measuring light
%   -4995: A2D overflow, measuring dark
%   -4994: Variable light level
%   -4993: Adaptive exposure time limit

if nargin < 1 || isempty(S)
    S = [380 5 81];
end

timeout = 300;
[rawspd,errcode] = PR705rawspd(timeout);

if isempty(rawspd) || isempty(errcode)
    error('Didn''t get a measurement reponse after %d seconds', timeout);
end

switch errcode
    case 0
        spd = PR705parsespdstr(rawspd, S);
    case {-4995,-4993} % too dark?
        spd = zeros(S(3), 1);
    case -4996 % too bright?
        [fixed_exposure, exposure_time] = exposure_params();
        if fixed_exposure
            fprintf(['\n\n*** Exposure time was too long and overflowed the meter!' ... 
                'Temporarily switching to adaptive exposure and trying again.***\n\n']);
            switch_to_exposure_mode(0);
            spd = PR705measspd(S);
            switch_to_exposure_mode(exposure_time);
        else
            error('The source is too bright for the meter!');
        end
    otherwise
        error('Received unhandled error code %d', errcode);
end

function [fixed,exposetime] = exposure_params() % A-58 to A-60 in PR-705 manual
PR705write('D601');
setupstr = PR705read(1, 100);
out = cell2mat(textscan(setupstr, '%*d,%*d,%*d,%*d,%*d,%*d,%d,%d')); %exposure mode and exposure time
fixed = out(1); exposetime = out(2);

function switch_to_exposure_mode(mode) % A-13 in PR-705 manual
PR705write(['S,,,,,' mode ',,,,,,']);
response = PR705read(1, 10);
success = strcmp(response, [' 0000' 13 10]);
if ~success
    error('Unable to change exposure mode!');
end
