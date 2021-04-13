function data = MeasureLuminancePrecision(meterType)
% data=MeasureLuminancePrecision([meterType=7])
%
% 'meterType' selects type of photometer. Defaults to 7 for CRS ColorCal2, see
% 'help CMCheckInit' for other supported devices.
%
% INSTRUCTIONS: Plug your photometer into your computer, carefully
% place your photometer stably against your computer's screen, set
% PARAMETERS (below), then run. The results (including the best-fitting
% n-bit-precision model) will be displayed as a graph in a figure window,
% and also saved in three files (in the same folder as this file) with
% filename extensions: png, fig, and mat. The filename describes the
% testing conditions, e.g.
% DenissMacBookPro5K-Dithering61696-o.use10Bits-LoadIdentityCLUT-Luminances8.fig
%
% EXPLANATION: Using Psychtoolbox SCREEN imaging, measures how precisely we
% can control display luminance. Loads identity into the Color Lookup Table
% (CLUT) and measures the luminance produced by each value loaded into a
% large identical patch of image pixels. (This program varies only the
% luminance, not hue, always varying the three RGB channels together, but
% the conclusion about bits of precision per channel almost certainly
% applies to general-purpose presentation of arbitrary RGB colors.) The
% attained precision will be achieved mostly by the digital-to-analog
% converter and, perhaps, partly through dither by the video driver. Since
% the 1980's most digital computer displays allocate 8 bits per color
% channel (R, G, B). In the past few years, some displays now accept 10 or
% more bits for each channel and pass that through from the pixel in memory
% through the color lookup table (CLUT) to the digital to analog converter
% that controls light output. In 2016-2017, Mario Kleiner enhanced The
% Psychtoolbox SCREEN function to allow specification of each color
% component (R G B) as a floating point number, where 0 is black and 1 is
% maximum output, so that your software, without change, will drive any
% display and benefit from as much precision as the display hardware and
% driver provide.
%
% Typically you'll run MeasureLuminancePrecision from the command line. It
% will make all the requested measurements and plot the results, including
% the best-fitting n-bit-precision model. Each figure is saved as both a
% FIG and PNG file, and the data are saved as a MAT file. The data are also
% returned as the output argument. It has luminance out "data.L" vs
% floating point color value "data.v".
%
% To use this program to measure the precision of your computer display you
% need three things:
% 1. MATLAB or Octave. http://mathworks.com , https://www.gnu.org/software/octave
% 2. The Psychtoolbox, free from http://psychtoolbox.org.
% 3. A photometer or colorimeter supported by CMCheckInit(), e.g., the CRS ColorCal2
% http://www.crsltd.com/tools-for-vision-science/light-measurement-display-calibation/colorcal-mkii-colorimeter/
% It's plug and play, taking power through its USB cable.
%
% As of April 2017, Apple documents (below) indicate that two currently
% available macOS computers attain 10-bit precision from pixel to display
% (in each of the three RGB channels): the Mac Pro and the iMac 27" retina
% desktop. From my testing, I add the Apple's high-end MacBook Pro laptop
% (Retina, 15-inch, Mid 2015). I tested my MacBook Pro (Retina, 15-inch,
% Mid 2015) and iMac (Retina 5K, 27-inch, Late 2014). Both use AMD drivers.
% Using MeasureLuminancePrecision, I have documented 11-bit luminance
% precision on both of these displays, enabling both o.use10Bits and
% o.useDithering,
% https://www.macrumors.com/2015/10/30/4k-5k-imacs-10-bit-color-depth-osx-el-capitan/
% https://developer.apple.com/library/content/samplecode/DeepImageDisplayWithOpenGL/Introduction/Intro.html#//apple_ref/doc/uid/TP40016622
% https://developer.apple.com/library/content/samplecode/DeepImageDisplayWithOpenGL/Introduction/Intro.html#//apple_ref/doc/uid/TP40016622
% https://macperformanceguide.com/blog/2016/20161127_1422-Apple2016MacBookPro-10-bit-color.html

% My Hewlett-Packard Z Book laptop running Linux also attains 10-bit
% luminance precision. I have not yet succeeded in getting dither to work
% on the Z Book. Thanks to my former student, H�rmet Yiltiz, for setting up
% the Z Book and getting 10-bit imaging to work, with help from Mario
% Kleiner.
%
% MacBook Pro driving NEC PA244UHD 4K display
% https://macperformanceguide.com/blog/2016/20161127_1422-Apple2016MacBookPro-10-bit-color.html
%
% PARAMETERS:
% o.luminances = number of luminances to measure, 3 s each.
% o.reciprocalOfFraction = list desired values, e.g. 1, 64, 128, 256.
% o.usePhotometer = 1 use ColorCAL II XYZ; 0 simulate 8-bit rendering.
% See SET PARAMETERS below.
%
% o.ditheringCode = 61696; Required for dither on my iMac and MacBook Pro.'
% For dither, the magic number 61696 is appropriate for the graphics chips
% belonging to the AMD Radeon "Southern Islands" gpu family. Such chips are
% used in the MacBook Pro (Retina, 15-inch, Mid 2015) (AMD Radeon R9 M290X)
% and the iMac (Retina 5K, 27-inch, Late 2014) (AMD Radeon R9 M370X). As
% far as I know, in April 2017, those are the only Apple Macs with AMD
% drivers, and may be the only Macs that support more-than-8-bit luminance
% precision.
%
% Denis Pelli, April 24, 2017
%

% History:
% 24-Apr-2017   dgp     Wrote original version.
% ??-???-2019   mk      Hacked it, improved it somewhere somewhat.
% 14-Feb-2021   mk      Included into Psychtoolbox as baseline for cleanup.

%% DITHERING NOTES
% (FROM MARIO) FOR HP Z Book "Sea Islands" GPU:
% 10 bpc panel dither setup code for the zBooks "Sea Islands" (CIK) gpu:
% http://lxr.free-electrons.com/source/drivers/gpu/drm/radeon/cik.c#L8814
% The constants which are or'ed / added together in that code are defined
% here:
% http://lxr.free-electrons.com/source/drivers/gpu/drm/radeon/cikd.h#L989
% I simply or'ed the proper constants to get the numbers i told you, so PTB
% replicates the Linux display drivers behaviour. As you can see there are
% many parameters one could tweak for any given display. E.g., add/drop
% FMT_FRAME_RANDOM_ENABLE, FMT_HIGHPASS_RANDOM_ENABLE, or
% FMT_RGB_RANDOM_ENABLE for extra entertainment value. It's somewhat of a
% black art. The gpu also has various temporal dithering modes with even
% more parameters, or combined spatio-temporal modes. Most of these are
% never used or even validated by gpu hardware vendors to do the right
% thing. All the variations will have different effects on different types
% of display panels, at different refresh rates and pixel densities, for
% different types of still images or animations, so a panel with a true
% native high bit depths is still a more deterministic thing that simulated
% high bit depths. I would use dithering only for high level stimuli with
% low spatial frequencies for that reason.

% DENIS: Must we call "PsychColorCorrection"? I'm already doing correction
% based on my photometry.

% MARIO: No. But it's certainly more convenient and faster, and very
% accurate. That's the recommended way to do gamma correction on > 8 bpc
% framebuffers. For testing it would be better to leave it out, so you use
% a identity mapping like when testing on the Macs.

% DENIS: Must we call "FinalFormatting"? Is the call to "FinalFormatting"
% just loading an identity gamma? Can I, instead, just use
% LoadFormattedGammaTable to load identity?

% MARIO: No, only if you want PTB to do high precision color/gamma
% correction via the modes and settings supported by
% PsychColorCorrection(). The call itself would simply establish an
% identity gamma "curve", however operating at ~ 23 bpc linear precision
% (32 bit floating point precision is about ~ 23 bit linear precision in
% the displayable color range of 0.0 - 1.0).

% -> Another thing you could test is if that laptop can drive a
% conventional 8 bit external panel with 12 or more bits via dithering. The
% gpu can do 12 bits in the 'EnableNative16BitFramebuffer' mode. So far i
% thought +2 extra bits would be all you could get via dithering, but after
% your surprising 11 bit result on your MacBookPro, with +3 extra bits, who
% knows if there's room for more?

% -> Yet another interesting option would be booting Linux on your iMac
% 2014 Retina 5k, again with the dither settings that gave you 11 bpc under
% macOS, and see if Linux in EnableNative16BitFramebuffer mode ! can
% squeeze out more than 11 bpc.

%% FROM MARIO

% Denis could you send me the .mat files with various measured curves? Also
% a measurement of the iMac Retina, just with 'EnableNative10Bit' mode, but
% *without* any of the special dither settings - after a machine reboot -
% would be good. I'd like to know how it behaves at Apples factory settings
% without our PTB specific hacks, as those are so machine specific.
% DONE: for MacBook Pro.

% Btw., so far i still didn't manage to replicate your 11 bpc with
% dithering finding on any AMD hardware + 8 bit display here, even with
% more modern AMD graphics cards, so i'm still puzzled by that result. I'll
% probably add some debug code to the next PTB beta for you to run on
% macOS, to dump some hardware settings, maybe that'd give some clues about
% how that 11 bpc instead of expected max 10 bpc happens.

%% SOFTWARE CLUT
% The following 4 parameters allow testing of the software CLUT, but that's
% a relatively unimportant option and not usable on the Z Book (restricted
% to 8 bit table), so you might as well not bother testing the software
% CLUT.
% My experiments with LoadNormalizedGammaTable indicate that it is accurate
% only for very smooth gamma functions. (Mario says this is because it
% stores only a functional approximation, not the requested values.) Thus
% fiddling with the CLUT is not a recommended way to achieve fine steps in
% luminance. It is generally better to leave the CLUT alone and adjust the
% pixel values.
%
%% SET PARAMETERS
% o.luminances = how many luminances are measured to produce your
% final graph. 32 is typically enough. The CRS photometer takes 3
% s/point.
% o.reciprocalOfFraction = reciprocal of the fraction of the full luminance
% range you want to explore. Setting it to 1 will explore the whole range.
% To demonstrate 10-bit precision over the whole range you'd need to test
% 2^10=1024 luminances, which will take a long time, 3,000 s, nearly an
% hour. Setting o.reciprocalOfFraction=256 will test only 1/256 of the range,
% which is enough to reveal whether there are any steps finer than one step
% at 8-bit precision. You can request several ranges by listing them, e.g.
% [1 128]. You'll get a graph for each. Each graph will use the specified
% number of luminances.
% o.wigglePixelNotCLUT = whether to vary the value of the pixel or CLUT.
% o.loadIdentityCLUT = whether to load an identity into CLUT.

%o.luminances=128; % Photometer takes 3 s/luminance. 128 luminances is enough for a pretty graph.
o.luminances=512; % Photometer takes 3 s/luminance. 512 luminances for a prettier graph.
o.reciprocalOfFraction= 32; % List one or more, e.g. 1, 128, 256.
o.vBase=.5;
o.useDithering=[]; % 1 enable. [] default. 0 disable.
o.nBits=10; % Enable this to get 10-bit (and better with dithering) performance.
o.useShuffle=0; % Randomize order of luminances to prevent systematic effect of changing background.
o.wigglePixelNotCLUT=1; % 1 is fine. The software CLUT is not important.
o.loadIdentityCLUT=1; % 1 is fine. This nullifies the CLUT.
o.useFractionOfScreen=0; % For debugging, reduce our window to expose Command Window.
o.useVulkan=0; % Use Vulkan display backend.

if IsOctave
    pkg load statistics;
end

if ~exist('regress') %#ok<EXIST>
    error('Required regress() function from Statistics toolbox missing!');
end

KbReleaseWait;

%% BEGIN
BackupCluts;
aborted = 0;

% Default to ColorCal2 - meterType 7:
if nargin < 1 || isempty(meterType)
    meterType = 7;
end

if meterType > 0
    % Open the colorimeter, or abort if not possible:
    CMCheckInit(meterType);
    o.usePhotometer = 1;
else
    o.usePhotometer = 0; % Simulate 8-bit rendering.
end

try
    %% OPEN WINDOW
    screen = 0;
    screenBufferRect = Screen('Rect',screen);

    PsychImaging('PrepareConfiguration');
    PsychImaging('AddTask','General','UseRetinaResolution');
    PsychImaging('AddTask','General','NormalizedHighresColorRange',1);

    if o.useVulkan
        PsychImaging('AddTask','General','UseVulkanDisplay');
    end

    switch o.nBits
        case 8
            % do nothing
        case 10
            PsychImaging('AddTask','General','EnableNative10BitFramebuffer');
        case 11
            PsychImaging('AddTask','General','EnableNative11BitFramebuffer');
        case 12
            if ~o.useVulkan && IsLinux && ~IsWayland
                PsychImaging('AddTask','General','EnableNative16BitFramebuffer');
            else
                PsychImaging('AddTask','General','EnableNative16BitFloatingPointFramebuffer');
            end
    end
    %if o.nBits >= 11; Screen('ConfigureDisplay','Dithering',screenNumber,61696); end

    if ~o.useFractionOfScreen
        window = PsychImaging('OpenWindow',screen,[1 1 1]);
    else
        window = PsychImaging('OpenWindow',screen,[1 1 1],round(o.useFractionOfScreen*screenBufferRect));
    end

    HideCursor(window);
    windowInfo=Screen('GetWindowInfo',window);

    switch(windowInfo.DisplayCoreId)
        % Choose the right magic dither code for the video driver. Currently
        % this works only for AMD drivers on  Apple's iMac and MacBook Pro,
        % and HP's Z Book. See Dithering Notes above.
        case 'AMD'
            displayEngineVersion=windowInfo.GPUMinorType/10;
            switch(round(displayEngineVersion))
                case 4
                    displayGPUFamily='Evergreen';
                    % Examples:
                    % AMD Radeon HD-5770 used in MacPro 2010.
                    o.ditheringCode=61696;
                case 6
                    displayGPUFamily='Southern Islands';
                    % Examples:
                    % AMD Radeon R9 M290X used in MacBook Pro (Retina, 15-inch, Mid 2015)
                    % AMD Radeon R9 M370X used in iMac (Retina 5K, 27-inch, Late 2014)
                    o.ditheringCode=61696;
                otherwise
                    displayGPUFamily='unknown';
            end
            fprintf('Display driver: %s version %.1f, "%s"\n',...
                windowInfo.DisplayCoreId,displayEngineVersion,displayGPUFamily);
    end

    if ~o.useDithering
        o.ditheringCode=0;
    end

    if isfinite(o.useDithering)
        fprintf('ConfigureDisplay Dithering %.0f\n',o.ditheringCode);
        % The documentation suggests that the first call enables, and the
        % second call sets the value.
        Screen('ConfigureDisplay','Dithering',screen,o.ditheringCode);
        Screen('ConfigureDisplay','Dithering',screen,o.ditheringCode);
    end

    if o.wigglePixelNotCLUT
        % Compare default CLUT with identity.
        gammaRead=Screen('ReadNormalizedGammaTable',window);
        maxEntry=size(gammaRead,1)-1;
        gamma=repmat(((0:maxEntry)/maxEntry)',1,3);
        delta=gammaRead(:,2)-gamma(:,2);
        fprintf('Difference between identity and read-back of default CLUT: mean %.9f, sd %.9f\n',mean(delta),std(delta));

        % Load identity hw lut once, as it can interfere with some precision modes
        % if done each flip, at least if PTB high precision hacks are used on the
        % AMD DC display driver:
        if o.loadIdentityCLUT
            Screen('LoadNormalizedGammaTable',window,gamma);
            Screen('Flip',window);
        end
    end

    %% MEASURE LUMINANCE AT EACH VALUE
    % Each measurement takes several seconds.
    clear data d
    t=GetSecs;
    nData=length(o.reciprocalOfFraction);
    for iData=1:nData
        d.fraction=1/o.reciprocalOfFraction(iData);
        v=max(0,o.vBase);
        if v+d.fraction>=1
            v=1-d.fraction;
        end

        newOrder=1:o.luminances;

        if o.useShuffle
            % Random order to prevent systematic effect of changing background.
            newOrder=Shuffle(newOrder);
        end

        % Repeat first measurement at end to estimate background drift.
        newOrder(end+1)=newOrder(1); %#ok<*AGROW>
        for ii=1:length(newOrder)
            i=newOrder(ii);
            g=v+d.fraction*(i-1)/(o.luminances-1);
            assert(g<=1+eps)
            d.v(i)=g;
            CLUTMapSize = 256;
            gamma=repmat(((0:CLUTMapSize-1)/(CLUTMapSize-1))',1,3);

            if o.wigglePixelNotCLUT
                Screen('FillRect',window, [g, g, g]);
            else
                % Note that this method will fail on many (most?) modern operating
                % systems and graphics cards to achieve the desired results! Not
                % recommended, only left for documentation!
                iPixel=126;
                for j=-4:4
                    gamma(1+iPixel+j,1:3)=[g g g];
                end

                Screen('LoadNormalizedGammaTable',window,gamma,1);
                Screen('FillRect',window,iPixel/(CLUTMapSize-1));
            end

            Screen('TextSize',window, 30);
            msg1=sprintf('Series %d of %d.\n',iData,nData);
            msg2=sprintf('%d luminances spanning 1/%.0f of digital range at %.2f.\n',o.luminances,1/d.fraction,d.v(1));
            msg3=sprintf('Luminance %d of %d.\n',ii,length(newOrder));
            msg4='Now measuring luminances. Will then analyze and plot the results.\n';
            DrawFormattedText(window, [msg1 msg2 msg3 msg4], 10, 30);
            Screen('Flip',window);

            if o.usePhotometer
                if ii==1
                    % Give the photometer time to react to new luminance.
                    WaitSecs(8);
                else
                    if o.useShuffle
                        WaitSecs(8);
                    else
                        WaitSecs(2);
                    end
                end

                L = GetLuminance(meterType); % Read photometer
            else
                % No photometer. Simulate 8-bit performance.
                L=200*round(g*255)/255;
                L=L-20*ii/512; % Simulate background drift.
            end

            if ii<length(newOrder)
                d.L(i)=L;
            else
                % Last iteration: Estimate and remove background drift.
                d.deltaL=L-d.L(newOrder(1));
                nn=newOrder(1:o.luminances);
                d.L(nn)=d.L(nn)-d.deltaL*(0:o.luminances-1)/o.luminances;
                fprintf('Corrected for luminance drift of %.2f%% during measurement.\n',100*d.deltaL/d.L(1));
            end

            if KbCheck
                aborted = 1;
                break;
            end
        end

        data(iData)=d;

        if KbCheck
            aborted = 1;
            break;
        end
    end

    t=(GetSecs-t)/length(data)/o.luminances;
catch
    if meterType > 0
        % Close the colorimeter:
        CMClose(meterType);
    end

    sca;
    psychrethrow(psychlasterror);
end

if meterType > 0
    % Close the colorimeter:
    CMClose(meterType);
end

sca;
close all;

if aborted
    fprintf('\n\nMeasurement script aborted. Bye!\n\n');
    return;
end

%% ANALYZE RESULTS
% We compare our data with the prediction for n-bit precision, and choose
% the best fit.
clear sd
for iData=1:length(data)
    d=data(iData);
    nMin=log2(1/d.fraction);
    vShift=-1:0.01:1;
    sd=ones(16,length(vShift))*nan;

    for bits=nMin:16
        for j=1:length(vShift)
            white=2^bits-1;
            v=d.v+vShift(j)*2^-bits;
            q=floor(v*white)/white;
            x=[ones(size(d.v))' q'];
            [~, ~, ~, ~, stats]=regress(d.L',x);
            sd(bits,j)=sqrt(stats(4));
        end
        fprintf('Modelbits= %d, minsd = %f\n', bits, min(sd(bits,:)))
    end

    minsd=min(min(sd));
    [bits, jShift]=find(sd==minsd,1);
    j=round((length(vShift)+1)/2);
    fprintf('Best fit: min sd %.3f at %d bits %.4f shift.\n', minsd, bits, vShift(jShift));
    data(iData).model.bits=bits;
    data(iData).model.vShift=vShift(jShift);
    data(iData).model.sd=sd(bits,jShift);
    white=2^bits-1;
    v=d.v+vShift(jShift)*2^-bits;
    q=floor(v*white)/white;
    x=[ones(size(d.v')) q'];
    b=regress(d.L',x);
    data(iData).model.b=b;
    data(iData).model.v=linspace(d.v(1),d.v(end),1000);
    v=data(iData).model.v+vShift(jShift)*2^-bits;
    q=floor(v*white)/white;
    data(iData).model.L=b(1)+b(2)*q;
end

%% PLOT RESULTS
o.luminances=length(data(1).L);

if exist('t','var')
    fprintf('Photometer took %.1f s/luminance.\n',t);
end

figure;
set(gcf,'PaperPositionMode','auto');
set(gcf,'Position',[0 300 320*length(data) 320]);

for iData=1:length(data)
    d=data(iData);
    subplot(1,length(data),iData)
    plot(d.v,d.L);
    hold on
    plot(d.model.v,d.model.L,'g');
    legend('data',sprintf('%.0f-bit model',d.model.bits));
    legend('boxoff');
    hold off
    ha=gca;

    if IsOctave
        set(ha, 'ticklength', [0.02, 0.025]);
    else
        ha.TickLength(1)=0.02;
    end

    title(sprintf('%.0f luminances spanning 1/%.0f of digital range',o.luminances,1/d.fraction));

    if o.wigglePixelNotCLUT
        xlabel('Pixel value');
    else
        xlabel('CLUT');
    end

    ylabel('Luminance (cd/m^2)');
    pbaspect([1 1 1]);
    computer=Screen('Computer');
    name=[computer.machineName ','];
    yLim=ylim;
    dy=-0.06*diff(yLim);
    y=yLim(2)+dy;
    xLim=xlim;
    x=xLim(1)+0.03*diff(xLim);
    text(x,y,name);
    name='';

    if isfinite(o.useDithering)
        name=sprintf('%sditheringCode %d, ',name,o.ditheringCode);
    end

    name=sprintf('%suse%iBits, ',name,o.nBits);

    y=y+dy;
    text(x,y,name);
    name='';

    if o.loadIdentityCLUT
        name=[name 'loadIdentityCLUT, '];
    end

    if ~o.usePhotometer
        name=[name 'simulating 8 bits, '];
    end

    name=sprintf('%sshift %.2f, ',name,d.model.vShift);
    name=sprintf('%smodel sd %.2f%%, ',name,100*d.model.sd/d.L(1));
    y=y+dy;
    text(x,y,name);
    name='';
    name=sprintf('%s%d luminances span a %.0f-bit prec. step at %.3f',name,o.luminances,log2(1/d.fraction),d.v(1));
    y=y+dy;
    text(x,y,name);
end

folder=fileparts(mfilename('fullpath'));
cd(folder);
name=computer.machineName;

if isfinite(o.useDithering)
    name=sprintf('%s-Dither%d',name,o.ditheringCode);
end

name=sprintf('%s-Use%iBits',name,o.nBits);

if ~o.usePhotometer
    name=[name '-Simulating8Bits'];
end

if o.useShuffle
    name=[name '-Shuffled'];
end

name=sprintf('%s-Luminances%d',name,o.luminances);
name=sprintf('%s-Span%.0fBitStep',name,log2(1/d.fraction));
name=sprintf('%s-At%.3f',name,d.v(1));
name=sprintf('%s-modelBits%.0f',name,d.model.bits);
name=strrep(name,'''',''); % Remove quote marks.
name=strrep(name,' ',''); % Remove spaces.
save([name '.mat'],'data'); % Save data as MAT file.
print(gcf,'-dpng',[name,'.png']); % Save figure as png file.

if IsOctave
    hgsave(gcf,[name,'.fig'],'-v7'); % Save figure as fig file.
else
    savefig(gcf,[name,'.fig'],'compact'); % Save figure as fig file.
end
end

%% GET LUMINANCE
function L = GetLuminance(meterType)
    % L = GetLuminance(o.usePhotometer)
    % Measure luminance (cd/m^2).

    if 0
        % Old code from Denis:
        % Cambridge Research Systems ColorCAL II XYZ Colorimeter.
        % http://www.crsltd.com/tools-for-vision-science/light-measurement-display-calibation/colorcal-mkii-colorimeter/nest/product-support
        persistent CORRMAT
        if isempty(CORRMAT)
            % Get ColorCAL II XYZ correction matrix (CRT=1; WLED LCD=2; OLED=3):
            CORRMAT=ColorCal2('ReadColorMatrix');
        end
        s = ColorCal2('MeasureXYZ');
        XYZ = CORRMAT(4:6,:) * [s.x s.y s.z]';
    else
        % New code: Uses selectable measurement device. In case of ColorCal2, uses
        % 2nd correction matrix rows 1:3 instead of Denis rows 4:6! My ColorCal2
        % does not contain a meaningful matrix in rows 4:6.
        XYZ = MeasXYZ(meterType);
    end

    L = XYZ(2);
end
