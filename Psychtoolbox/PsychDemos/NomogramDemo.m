% NomogramDemo
%
% Compare shapes of different photoreceptor nomograms.
%
% 7/8/03  dhb  Wrote it.
% 7/16/03 dhb  Add Stockman Sharpe nomogram.
% 8/13/12 dhb  Removed some half done stuff at the end which was not very 
%              useful.

% Clear out
clear all; close all

% Set parameters
S = [380 5 81];
lambdaMax = [440 530 560]';
nSpectra = length(lambdaMax);

% Compute all the nomograms
T_Baylor = PhotopigmentNomogram(S,lambdaMax,'Baylor');
T_Dawis = PhotopigmentNomogram(S,lambdaMax,'Dawis');
T_Govardovskii = PhotopigmentNomogram(S,lambdaMax,'Govardovskii');
T_Lamb = PhotopigmentNomogram(S,lambdaMax,'Lamb');
T_SS = PhotopigmentNomogram(S,lambdaMax,'StockmanSharpe');

% Plot all nomograms in absorbance and absorptance
for i = 1:nSpectra
	warning('off');
	figure(i); clf; set(gcf,'Position',[100 400 700 300]);
	subplot(1,2,1); hold on
	set(plot(SToWls(S),T_Baylor(i,:),'g'),'LineWidth',2);
	set(plot(SToWls(S),T_Dawis(i,:),'b'),'LineWidth',2);
	set(plot(SToWls(S),T_Govardovskii(i,:),'k'),'LineWidth',2);
	set(plot(SToWls(S),T_Lamb(i,:),'r'),'LineWidth',2);
	set(plot(SToWls(S),T_SS(i,:),'y'),'LineWidth',2);
	set(title('Linear'),'FontSize',14);
	set(xlabel('Wavelength (nm)'),'FontSize',12);
	set(ylabel('Absorbance'),'FontSize',12);
	axis([300 800 0 1]);
	subplot(1,2,2); hold on
	set(plot(SToWls(S),log10(T_Baylor(i,:)),'g'),'LineWidth',2);
	set(plot(SToWls(S),log10(T_Dawis(i,:)),'b'),'LineWidth',2);
	set(plot(SToWls(S),log10(T_Govardovskii(i,:)),'k'),'LineWidth',2);
	set(plot(SToWls(S),log10(T_Lamb(i,:)),'r'),'LineWidth',2);
	set(plot(SToWls(S),log10(T_SS(i,:)),'y'),'LineWidth',2);
	set(title('Log'),'FontSize',14);
	set(xlabel('Wavelength (nm)'),'FontSize',12);
	set(ylabel('Log Absorbance'),'FontSize',12);
	axis([300 800 -4 0]);
	% Donot do this here: warning('on');
end

return
