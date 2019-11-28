function DrawChromaticity(type)

% This function provides a standard method for drawing a chromaticity
% diagram with a coloured spectral locus. The colouring is only rough,
% since monitors will not be able to reproduce spectral colours, but should
% provide an indication as to the orientation of colourspace.

% Accepted inputs are:
% - '1931' provides the CIE 1931 chromaticity space
% - '1964' provides the CIE 1964 chromaticity space
% - 'Phys2' provides an xy chromaticity space based on the CIE Phys 2 deg observer
% - 'Phys10' provides an xy chromaticity space based on the CIE Phys 2 deg observer
% - 'uv' provides CIE 1960 UCS diagram (obsolete)
% - 'upvp' provides the CIE 1976 UCS diagram, aka u'v', aka u-prime,v-prime
% - 'MB2'  provides MacLeod-Boynton-type diagram for 2degree observer,
%   based on Stockman/Sharpe fundamentals
% - 'MB10' - as above for 10degree

% Example usage:
% DrawChromaticity('1931')

% 10-Aug-2019 dg  Written by DG
% 28-Nov-2019 mk  Fixup naming of file and function so Octave does not complain
%                 and online help generator does not choke on it. Also fix Octave
%                 load path warning.

if ~exist('type','var') % if a type of colourspace is not specified, assume CIE 1931
    type = '1931';
end

if IsOctave
    % Suppress Octave warning about the load instruction without path below:
    warning('off', 'Octave:data-file-in-path', 'local');
end

%% Calculate sRGB values

% This loop computes the colour for each line segment of the spectral locus.
% Specifically - it computes the RGB for each point on the spectral locus,
% and takes the average of two points to be the colour of the line
% connecting them.
load T_xyz1931 T_xyz1931 S_xyz1931  % CMF: 1931 2deg
sRGB_SL = XYZToSRGBPrimary(T_xyz1931); % sRGB Spectral Locus
sRGB_SL(sRGB_SL>1) = 1; %Threshold values to between 0 and 1
sRGB_SL(sRGB_SL<0) = 0;
for i = 1:3
    for j = 1:size(sRGB_SL,2)-1
        sRGBLine(i,j) = (sRGB_SL(i,j)+sRGB_SL(i,j+1))/2;
    end
end

%% Compute locus

if strcmp(type,'1931') % CIE 1931 chromaticity space
    SL = [T_xyz1931(1,:)./sum(T_xyz1931);T_xyz1931(2,:)./sum(T_xyz1931)]; % Spectral Locus
elseif strcmp(type,'1964') % CIE 1964 chromaticity space
    load T_xyz1964.mat T_xyz1964 S_xyz1964
    T_xyz = SplineCmf(S_xyz1964,T_xyz1964,S_xyz1931); %resampling so that I can use the old sRGBs that I already calculated, and keep the appearance comparable accross diagrams. Currently 1931 and 1964 are of same range and interval, but including this line for futureproofing
    SL = [T_xyz(1,:)./sum(T_xyz);T_xyz(2,:)./sum(T_xyz)];
elseif strcmp(type,'Phys2') %CIE Phys 2 degree
    load T_xyzCIEPhys2.mat T_xyzCIEPhys2 S_xyzCIEPhys2
    T_xyz = SplineCmf(S_xyzCIEPhys2,T_xyzCIEPhys2,S_xyz1931); %resampling so that I can use the old sRGBs that I already calculated, and keep the appearance comparable accross diagrams. Currently 1931 and 1964 are of same range and interval, but including this line for futureproofing
    SL = [T_xyz(1,:)./sum(T_xyz);T_xyz(2,:)./sum(T_xyz)];
elseif strcmp(type,'Phys10') %CIE Phys 10 degree
    load T_xyzCIEPhys10.mat T_xyzCIEPhys10 S_xyzCIEPhys10
    T_xyz = SplineCmf(S_xyzCIEPhys10,T_xyzCIEPhys10,S_xyz1931); %resampling so that I can use the old sRGBs that I already calculated, and keep the appearance comparable accross diagrams. Currently 1931 and 1964 are of same range and interval, but including this line for futureproofing
    SL = [T_xyz(1,:)./sum(T_xyz);T_xyz(2,:)./sum(T_xyz)];
elseif strcmp(type,'uv') % CIE uv (1960)
    SL = xyTouv([T_xyz1931(1,:)./sum(T_xyz1931);T_xyz1931(2,:)./sum(T_xyz1931)],1);
elseif strcmp(type,'upvp') % CIE u' v' (u-prime, v-prime)
    SL = xyTouv([T_xyz1931(1,:)./sum(T_xyz1931);T_xyz1931(2,:)./sum(T_xyz1931)]);
elseif strcmp(type,'MB2') % MacLeod Boynton 2degree
    load T_cones_ss2.mat T_cones_ss2 S_cones_ss2
    load T_CIE_Y2.mat T_CIE_Y2 S_CIE_Y2
    T_c = SplineCmf(S_cones_ss2,T_cones_ss2,S_xyz1931); %resampling so that I can use the old sRGBs that I already calculated, and keep the appearance comparable accross diagrams
    T_C = SplineCmf(S_CIE_Y2,T_CIE_Y2,S_xyz1931);
    SL = LMSToMacBoyn(T_c,T_c,T_C);
elseif strcmp(type,'MB10') % MacLeod Boynton 10degree
    load T_cones_ss10.mat T_cones_ss10 S_cones_ss10
    load T_CIE_Y10.mat T_CIE_Y10 S_CIE_Y10
    T_c = SplineCmf(S_cones_ss10,T_cones_ss10,S_xyz1931); %resampling so that I can use the old sRGBs that I already calculated, and keep the appearance comparable accross diagrams
    T_C = SplineCmf(S_CIE_Y10,T_CIE_Y10,S_xyz1931);
    SL = LMSToMacBoyn(T_c,T_c,T_C);
else
    error('input not recognised')
end

%% Plot locus

for i = 1:size(sRGB_SL,2)-1
    plot([SL(1,i),SL(1,i+1)],[SL(2,i),SL(2,i+1)],...
        'Color',sRGBLine(:,i),...
        'LineWidth',2,...
        'HandleVisibility','off'); % This means that it won't show up on legends
    hold on % hold on goes here so that this function can create a new figure if one does not exist, or can overlay an existing figure
    box off % gets rid of outer edge tick marks
end

%% Axis labels

if strcmp(type,'1931') % CIE 1931 chromaticity space
    xlabel('x_{1931}')
    ylabel('y_{1931}')
elseif strcmp(type,'1964') % CIE 1964 chromaticity space
    xlabel('x_{1964}')
    ylabel('y_{1964}')
elseif strcmp(type,'Phys2') % CIE 1964 chromaticity space
    xlabel('x_{F}') % `subscript F denoting �cone-fundamental-based�' (CIE 170-2:2015, p.3)
    ylabel('y_{F}')
elseif strcmp(type,'Phys10') % CIE 1964 chromaticity space
    xlabel('x_{F,10}') % `subscript F denoting �cone-fundamental-based�' (CIE 170-2:2015, p.3)
    ylabel('y_{F,10}')
elseif strcmp(type,'uv') % CIE u' v' (u-prime, v-prime)
    xlabel('u_{1960}')
    ylabel('v_{1960}')
elseif strcmp(type,'upvp') % CIE u' v' (u-prime, v-prime)
    xlabel('u''')
    ylabel('v''')
elseif strcmp(type,'MB2') % MacLeod Boynton 2degree
    xlabel('{\itl}_{MB,2}')
    ylabel('{\its}_{MB,2}')
elseif strcmp(type,'MB10') % MacLeod Boynton 10degree
    xlabel('{\itl}_{MB,10}')
    ylabel('{\its}_{MB,10}')
end

if any([strcmp(type,'1931'),strcmp(type,'upvp'),strcmp(type,'Phys2'),strcmp(type,'Phys10')])
    axis equal
    axis([0 1 0 1])
elseif or(strcmp(type,'MB2'),strcmp(type,'MB10'))
    axis([0.5 1 0 1])
    % axis not made equal because the relationship between axes in MB is arbitrary
end

end
