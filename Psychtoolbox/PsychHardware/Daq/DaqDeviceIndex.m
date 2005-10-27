function daq=DaqDeviceIndex
% daq=DaqDeviceIndex
% Returns a list of all your USB-1208FS daqs. 
% 
% TECHNICAL NOTE: When we call PsychHID('Devices'), each USB-1208FS box
% presents itself as four HID "devices" sharing the same serial number. They
% are interfaces 0,1,2,3. They usually appear in reverse order in the
% device list. Nearly all the USB-1208FS commands use only interface 0, so
% we will select that one to represent the whole. All our Daq routines
% expect to receive just the interface 0 device as a passed designator. The
% few routines that need to access the other interfaces do so
% automatically.
% See also Daq, DaqFunctions, DaqPins, TestDaq, TestPsychHid,
% DaqDeviceIndex, DaqDIn, DaqDOut, DaqAIn, DaqAOut, DaqAInScan,DaqAOutScan.

% 4/15/05 dgp Wrote it.
% 8/26/05 dgp Added support for new "USB" name, as suggested by Steve Van Hooser <vanhooser@neuro.duke.edu>.
% 8/26/05 dgp Incorporated bug fixes (for compatibility with Mac OS X Tiger) 
%             suggested by Jochen Laubrock <laubrock@rz.uni-potsdam.de>
%             and Maria Mckinley <parody@u.washington.edu>. 
%             The reported number of outputs of the USB-1208FS has changed in Tiger.
%             http://groups.yahoo.com/group/psychtoolbox/message/3610
%             http://groups.yahoo.com/group/psychtoolbox/message/3614


devices=PsychHID('Devices');
daq=[];
for i=1:length(devices)
	product=devices(i).product;
    if (streq(product,'PMD-1208FS')|streq(product,'USB-1208FS')) & devices(i).outputs>=70
        daq(end+1)=i;
    end
end
