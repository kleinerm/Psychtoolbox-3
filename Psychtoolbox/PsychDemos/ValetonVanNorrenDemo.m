% ValetonVanNorrenDemo
%
% Demonstrate computations with Valeton/Van Norren (1983)
% cone adaptation model.
%
% Right now we just know how to get the model's parameters
% from background isomerization rates.  Still need to implement
% the adaptation model itself.
%
% 07/26/03  dhb  Wrote it.
% 05/26/14  dhb  Dusted this off so it runs again.

% Clear
clear; close all;

% Set some photoreceptor properties.  These should match up
% with the parameters used in the Valeton Van Norren experiment,
% and thus ideally should describe the properties of the Monkey eye.
% Here we start with reasonable human parameters and then muck
% around by overriding the defaults before filling in the structure.
photoreceptors = DefaultPhotoreceptors('LivingHumanFovea');
photoreceptors.nomogram.source = 'Govardovskii';
photoreceptors.lensDensity.source = 'StockmanSharpe';
photoreceptors.macularPigmentDensity.source = 'None';
photoreceptors.eyeLengthMM.value = EyeLength('Rhesus','Packer');
photoreceptors.ISdiameter.value = [2 2 2]';
photoreceptors = rmfield(photoreceptors,'OSlength');
photoreceptors = rmfield(photoreceptors,'specificDensity');
photoreceptors.axialDensity.value = PhotopigmentAxialDensity({'LCone' 'MCone' 'SCone'},'Human','StockmanSharpe');
photoreceptors = FillInPhotoreceptors(photoreceptors);
LMRatio = 2;

% Define common wavelength sampling for this script.
S = photoreceptors.nomogram.S;

% Make a plot showing the effective photoreceptor sensitivities in quantal
% units, expressed as probability of isomerization.
figure; hold on
set(plot(SToWls(S),photoreceptors.isomerizationAbsorptance(1,:),'r'),'LineWidth',2);
set(plot(SToWls(S),photoreceptors.isomerizationAbsorptance(2,:),'g'),'LineWidth',2);
set(plot(SToWls(S),photoreceptors.isomerizationAbsorptance(3,:),'b'),'LineWidth',2);
set(title('Isomerization Absorptance'),'FontSize',14);
set(xlabel('Wavelength (nm)'),'FontSize',12);
set(ylabel('Probability'),'FontSize',12);
axis([300 800 0 1]);

% Make up a triplet of isomerization rates to get parameters for.
isoPerConeSec = [2.5e3 1.7e4 1.3e2]';

% Compute Valeton and VanNorren adaptation parameters, and report relation between their
% trolands and isomerizations used in the splining.
for i = 1:length(isoPerConeSec)
	params = ValetonVanNorrenParams(log10(isoPerConeSec(i)),photoreceptors,'Photopic',LMRatio);
	logSigmaAlpha(i) = params.logSigmaAlpha;
	gamma(i) = params.gamma;
end
isosPerTrolandVVN = (10^params.logBackgroundIsoRates(1))/(10^params.logBackgroundTds(1));

% Print out a table summarizing the calculation.
fprintf('***********************************************\n');
fprintf('Valeton-Van Norren calculations\n');
fprintf('Computed %0.3g isos/troland for their experimental conditions\n',isosPerTrolandVVN);
fprintf('\n');
fprintf('Calculations of isos/troland done using:\n');
fprintf('\t%s estimates for photoreceptor IS diameter\n',photoreceptors.ISdiameter.source);
if (isfield(photoreceptors,'OSlength'))
    fprintf('\t%s estimates for photoreceptor OS length\n',photoreceptors.OSlength.source);
end
if (isfield(photoreceptors,'specificDensity'))
    fprintf('\t%s estimates for receptor specific density\n',photoreceptors.specificDensity.source);
end
fprintf('\t%s photopigment nomogram\n',photoreceptors.nomogram.source);
fprintf('\t%s estimates for lens density\n',photoreceptors.lensDensity.source);
fprintf('\t%s estimates for macular pigment density\n',photoreceptors.macularPigmentDensity.source);
fprintf('\t%g mm for axial length of eye\n',photoreceptors.eyeLengthMM.value);
fprintf('\tLM cone ratio: %g\n',LMRatio);
fprintf('\n');
fprintf('Photoreceptor Type             |\t       L\t       M\t     S\n');
fprintf('______________________________________________________________________________________\n');
fprintf('\n');
fprintf('Lambda max                     |\t%8.1f\t%8.1f\t%8.1f\t nm\n',photoreceptors.nomogram.lambdaMax);
if (isfield(photoreceptors,'OSlength') && ~isempty(photoreceptors.OSlength.value))
    fprintf('Outer Segment Length           |\t%8.1f\t%8.1f\t%8.1f\t um\n',photoreceptors.OSlength.value);
end
if (isfield(photoreceptors,'OSdiameter') && ~isempty(photoreceptors.OSdiameter.value))
    fprintf('Outer Segment Diameter         |\t%8.1f\t%8.1f\t%8.1f\t um\n',photoreceptors.OSdiameter.value);
end
fprintf('Inner Segment Diameter         |\t%8.1f\t%8.1f\t%8.1f\t um\n',photoreceptors.ISdiameter.value);
fprintf('\n');
if (isfield(photoreceptors,'specificDensity') && ~isempty(photoreceptors.specificDensity.value))
    fprintf('Axial Specific Density         |\t%8.3f\t%8.3f\t%8.3f\t /um\n',photoreceptors.specificDensity.value);
end
fprintf('Axial Optical Density          |\t%8.3f\t%8.3f\t%8.3f\n',photoreceptors.axialDensity.value);
fprintf('Bleached Axial Optical Density |\t%8.3f\t%8.3f\t%8.3f\n',photoreceptors.axialDensity.bleachedValue);
fprintf('Peak isomerization prob.       |\t%8.3f\t%8.3f\t%8.3f\n',max(photoreceptors.isomerizationAbsorptance,[],2));
fprintf('______________________________________________________________________________________\n');
fprintf('\n');
fprintf('Valeton-VanNorren parameters   |\n')
fprintf('Log10 isomerizations           |\t%8.2f\t%8.2f\t%8.2f\t log10(iso)/photoreceptor-sec\n',...
	 log10(isoPerConeSec));
fprintf('logSigmaAlpha                  |\t%8.2f\t%8.2f\t%8.2f\n',logSigmaAlpha);
fprintf('gamma                          |\t%8.2f\t%8.2f\t%8.2f\n',gamma);
fprintf('______________________________________________________________________________________\n');
