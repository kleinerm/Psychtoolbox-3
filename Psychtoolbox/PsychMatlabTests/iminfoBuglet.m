function iminfoBuglet
% Mathworks Case ID 187040, reported 2/99
% 
% IMINFO gives misleading error message.
% 
% >There's a minor bug in the error reporting in imjpginfo.m, which is called by
% >IMFINFO
% >
% >'bill.jpeg' is a CMYK color jpeg file that i created in photoshop. Matlab
% >doesn't know how to read that, which is ok. Your imjpg.mex file does the
% >right thing, returning an empty 'info', and a 'msg' of 'Jpeg image is
% >neither grayscale or RGB.'
% >
% >However, your imjpginfo.m then fails, with the misleading error message:
% >
% >+imfinfo('bill.jpg','jpg')
% >??? Reference to non-existent field 'BitDepth'.
% >
% >Error in ==> Savoca:Applications:Matlab 
% >5.2:Toolbox:matlab:iofun:private:imjpginfo.m
% >On line 50  ==>     if (info.BitDepth == 8)
% >
% >The cleanest fix for this buglet would be to check that 'msg' is empty
% >before accessing any fields of 'info'.
% 
% 
% Denis,
% 
% Thanks for your report and detailed diagnosis of the JPEG imfinfo problem.
% I'll fix it soon.  The fix won't make it into the next release, however,
% because development has been completed on it.
% 
% Thanks again,
% 
% Steve 
% 
% --
% Steve Eddins, image processing specialist           eddins@mathworks.com
% The MathWorks, Inc.                                 http://www.mathworks.com
