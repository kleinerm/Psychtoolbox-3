portaudio_unpatched_except4OSX.zip is the original portaudio version which we use as
basis for our own enhancements. Well, almost - the patches for OS/X Coreaudio are 
partially applied to this codebase. 
To build portaudio from scratch, you'll need to unpack this zip file, then replace 
the files inside the unzipped source tree by the patched files in this folder, 
overwriting the originals. For Windows+ASIO you'll need to download the freely available
Steinberg ASIO SDK from their website and copy its files into the proper portaudio 
folders and set the proper build flags, as described in portaudio's documentation 
Finally, build the thing. 
This zip file is just here to document our 'baseline'. Our portaudio should more 
closely track the official upstream portaudio engine, but that would require us 
to first get our patches accepted for upstream. Something to do in the future... 
