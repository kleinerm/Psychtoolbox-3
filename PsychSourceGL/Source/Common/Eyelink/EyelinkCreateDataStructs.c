/*
	/Psychtoolbox-3/PsychSourceGL/Source/Common/Eyelink/EyelinkCreateDataStructs.c
 
	PROJECTS: Eyelink 
 
	AUTHORS:
        	cburns@berkeley.edu             cdb
                E.Peters@ai.rug.nl              emp
                f.w.cornelissen@med.rug.nl      fwc
                e_flister@yahoo.com             edf
                brian@sr-research.com           br
                mario.kleiner.de@gmail.com      mk

	PLATFORMS:	All

	HISTORY:
                2001            emp     created
                11/23/05        cdb     adapted for OSX.
                30/10/06        fwc     added CreateMXFSampleRaw
                22/03/09        edf     added fields to CreateMXFSampleRaw
                27/03/09        edf     added FLOAT_TIME (**BACKWARDS INCOMPATIBLE**) and TODO discussion
                21/10/24    br       removed MXISample & MXIEvent as also removed from API
                30/10/24    br       replace FSAMPLE with FSAMPLE2 for EL3 support
                06/11/24    br       replace FEVENT with FEVENT2 for EL3 support
                04/08/26        mk      removed CreateMXISample and CreateMXIEvent, no longer supported by 2026 SDK

     TODO: 
        matlab sucks at collecting together an array made up of the elements at a particular position 
          in each of the values of a given field in a struct array.
        the most efficient option is UGLY (and hardly efficient):
          tmp=cell2mat({s.px}'); %the px's are rows, as in an MXFSample
          tmp(:,1) %collect the first element of all the px's into a vector 
          % note a for loop w/preallocation is inexplicably slightly faster -- must be noticed by their JIT accelerator -- but still too slow
          see the discussion section here: http://blogs.mathworks.com/loren/2006/06/02/structures-and-comma-separated-lists/
        this is a common thing users would like to do with our output.  
        efficiency is at a premium if we don't want to be the cause of missed frame deadlines.
        we should make it easier on them by only providing scalar fields, providing separate fields for each eye.
        but this will not be backwards compatible -- possibly have a preference setting to allow this scalar-style output.
 
        switch FSAMPLE and FEVENT to DSAMPLE and DEVENT
            the new 2kHz mode of trackers means times (in ms) don't fit into unit32's, 
            so these structures represent times as doubles.  using F* means we need
            to call the macro FLOAT_TIME to get the true time.
 
        there is a lot of violation of "once and only once" in this file, and it wastes space by making everything doubles. 
        space may be at a premium if the user is saving all the samples for a trial. 
            at 2kHz, each second of full samples including raw fields is 768kB in a double representation, 
            but only 520kB in native types (if we convert their floats to doubles -- 328kB if we use singles instead).  
            so correctly typed fields allows 32% longer trials (57% if we use singles).
            is there any reason to suspect we should use doubles instead of singles for their floats?
			***mario points out that octave and very old matlabs don't have singles, and the Psych*** abstraction layer realy only wants you to use doubles or uint8's
			***also this should not be default behavior, but specially requested (so it is backwards compatible)
			***edf agrees with not making it the default, but thinks this is too big of a space-waste to not offer for users w/recent matlabs
			***asking users to do the type conversion more than doubles the time they have to sacrifice to eyetracking per frame (but it is still fast, like 0.1ms)

        consider redesigning thusly:
 
			typedef struct {
	         char *name;
             int type;
             int arity;
            } fields_s;

            const fields_s fields[] = {
                       {"time",     mxUINT32_CLASS, 1}, //change to double if use FLOAT_TIME
                       {"type",     mxINT16_CLASS,  1},
                       {"flags",    mxUINT16_CLASS, 1},
                       {"px",       mxDOUBLE_CLASS, 2},
                       {"py",       mxDOUBLE_CLASS, 2},
                       {"hx",       mxDOUBLE_CLASS, 2},
                       {"hy",       mxDOUBLE_CLASS, 2},
                       {"pa",       mxDOUBLE_CLASS, 2},
                       {"gx",       mxDOUBLE_CLASS, 2},
                       {"gy",       mxDOUBLE_CLASS, 2},
                       {"rx",       mxDOUBLE_CLASS, 1},
                       {"ry",       mxDOUBLE_CLASS, 1},
                       {"status",   mxUINT16_CLASS, 1},
                       {"input",    mxUINT16_CLASS, 1},
                       {"buttons",  mxUINT16_CLASS, 1},
                       {"htype",    mxINT16_CLASS,  1},
                       {"hdata",    mxINT16_CLASS,  8},
                                    //raw fields
                       {"raw_pupil_x",          mxDOUBLE_CLASS, 1},
                       {"raw_pupil_y",          mxDOUBLE_CLASS, 1},
                       {"raw_cr_x",             mxDOUBLE_CLASS, 1},
                       {"raw_cr_y",             mxDOUBLE_CLASS, 1},
                       {"pupil_area",           mxUINT32_CLASS, 1},
                       {"cr_area",              mxUINT32_CLASS, 1},
                       {"pupil_dimension_w",	mxUINT32_CLASS, 1},
                       {"pupil_dimension_h",    mxUINT32_CLASS, 1},
                       {"cr_dimension_w",       mxUINT32_CLASS, 1},
                       {"cr_dimension_h",       mxUINT32_CLASS, 1},
                       {"window_position_x",	mxUINT32_CLASS, 1},
                       {"window_position_y",	mxUINT32_CLASS, 1},
                       {"pupil_cr_x",           mxDOUBLE_CLASS, 1},
                       {"pupil_cr_y",           mxDOUBLE_CLASS, 1},
                       {"cr_area2",             mxUINT32_CLASS, 1},
                       {"raw_cr2_x",            mxDOUBLE_CLASS, 1},
                       {"raw_cr2_y",            mxDOUBLE_CLASS, 1}};
            
            void *tmp;
            for(i=0;i<numFields;i++){    
              fieldVal = mxCreateNumericMatrix(1,fields[i].arity, fields[i].type, mxREAL);
              switch (i){
                case 0:  tmp = &(fs.time);        break;
                case 1:  tmp = &(fs.type);        break;
                case 2:  tmp = &(fs.flags);       break;
                case 3:  tmp =   fs.px;           break; //don't take address of arrays
                case 4:  tmp =   fs.py;           break;
                case 5:  tmp =   fs.hx;           break;
                case 6:  tmp =   fs.hy;           break;
                case 7:  tmp =   fs.pa;           break;
                case 8:  tmp =   fs.gx;           break;
                case 9:  tmp =   fs.gy;           break;
                case 10: tmp = &(fs.rx);          break;
                case 11: tmp = &(fs.ry);          break;
                case 12: tmp = &(fs.status);      break;
                case 13: tmp = &(fs.input);       break;
                case 14: tmp = &(fs.buttons);     break;
                case 15: tmp = &(fs.htype);       break;
                case 16: tmp =   fs.hdata;        break;
                // the rest is raw data
                case 17: tmp = &(raw.raw_pupil[0]);         break;
                case 18: tmp = &(raw.raw_pupil[1]);         break;
                case 19: tmp = &(raw.raw_cr[0]);            break;
                case 20: tmp = &(raw.raw_cr[1]);            break;
                case 21: tmp = &(raw.pupil_area);           break;
                case 22: tmp = &(raw.cr_area);              break;
                case 23: tmp = &(raw.pupil_dimension[0]);	break;
                case 24: tmp = &(raw.pupil_dimension[1]);	break;
                case 25: tmp = &(raw.cr_dimension[0]);      break;
                case 26: tmp = &(raw.cr_dimension[1]);      break;
                case 27: tmp = &(raw.window_position[0]);	break;
                case 28: tmp = &(raw.window_position[1]);	break;
                case 29: tmp = &(raw.pupil_cr[0]);          break;
                case 30: tmp = &(raw.pupil_cr[1]);          break;  
              }
              memcpy(mxGetData(fieldVal), tmp, sizeof(*tmp)*extendedData[i].arity);  //hmm, can't sizeof() on a dereferenced void*.  any solution other than repeating the field name?
            }
 */
#include "PsychEyelink.h"

/*
ROUTINE: CreateMXFSample
PURPOSE:
   copys a FSAMPLE structure to a matlab FSAMPLE structure
*/
mxArray *CreateMXFSample(const FSAMPLE *fs)
{
   const char *fieldNames[] = {"time","type","flags","px","py","hx","hy","pa","gx","gy",
                               "rx","ry","status","input","buttons","htype","hdata"};
   const int fieldCount=sizeof(fieldNames)/sizeof(*fieldNames);   
   mxArray *struct_array_ptr, *mx; 
   
   if ((*fs).type != SAMPLE_TYPE){
     // PrintfExit("CreateMXFSample: wrong pointer argument\n");
     printf("CreateMXFSample: wrong pointer argument\n");
	 return(NULL);
	}
 
   /* Create a 1-by-1 structmatrix. */  
   struct_array_ptr = mxCreateStructMatrix(1,1,fieldCount,fieldNames);   
   if (struct_array_ptr == NULL)
      PrintfExit("Could not create struct matrix (probably out of memory)\n");

   mx = mxCreateDoubleMatrix(1,1,mxREAL);
   mxGetPr(mx)[0] = FLOAT_TIME(fs); //backwards incompatible change from (*fs).time -- with new 2kHz sample rates, time (in ms) can be fractional, this macro checks fs->flags to see if the uint32 fs->time needs an extra .5 ms
   mxSetField(struct_array_ptr,0,"time",mx);

   mx = mxCreateDoubleMatrix(1,1,mxREAL);
   mxGetPr(mx)[0] = (*fs).type;
   mxSetField(struct_array_ptr,0,"type",mx);

   mx = mxCreateDoubleMatrix(1,1,mxREAL);
   mxGetPr(mx)[0] = (*fs).flags;
   mxSetField(struct_array_ptr,0,"flags",mx);
   
   mx = mxCreateDoubleMatrix(1,2,mxREAL);
   mxGetPr(mx)[0] = (*fs).px[0];
   mxGetPr(mx)[1] = (*fs).px[1];
   mxSetField(struct_array_ptr,0,"px",mx);
      
   mx = mxCreateDoubleMatrix(1,2,mxREAL);
   mxGetPr(mx)[0] = (*fs).py[0];
   mxGetPr(mx)[1] = (*fs).py[1];
   mxSetField(struct_array_ptr,0,"py",mx);
   
   mx = mxCreateDoubleMatrix(1,2,mxREAL);
   mxGetPr(mx)[0] = (*fs).hx[0];
   mxGetPr(mx)[1] = (*fs).hx[1];
   mxSetField(struct_array_ptr,0,"hx",mx);
         
   mx = mxCreateDoubleMatrix(1,2,mxREAL);
   mxGetPr(mx)[0] = (*fs).hy[0];
   mxGetPr(mx)[1] = (*fs).hy[1];
   mxSetField(struct_array_ptr,0,"hy",mx); 

   mx = mxCreateDoubleMatrix(1,2,mxREAL);
   mxGetPr(mx)[0] = (*fs).pa[0];
   mxGetPr(mx)[1] = (*fs).pa[1];
   mxSetField(struct_array_ptr,0,"pa",mx);    
   
   mx = mxCreateDoubleMatrix(1,2,mxREAL);
   mxGetPr(mx)[0] = (*fs).gx[0];
   mxGetPr(mx)[1] = (*fs).gx[1];
   mxSetField(struct_array_ptr,0,"gx",mx);    
   
   mx = mxCreateDoubleMatrix(1,2,mxREAL);
   mxGetPr(mx)[0] = (*fs).gy[0];
   mxGetPr(mx)[1] = (*fs).gy[1];
   mxSetField(struct_array_ptr,0,"gy",mx);

   mx = mxCreateDoubleMatrix(1,1,mxREAL);
   mxGetPr(mx)[0] = (*fs).rx;
   mxSetField(struct_array_ptr,0,"rx",mx);

   mx = mxCreateDoubleMatrix(1,1,mxREAL);
   mxGetPr(mx)[0] = (*fs).ry;
   mxSetField(struct_array_ptr,0,"ry",mx);

   mx = mxCreateDoubleMatrix(1,1,mxREAL);
   mxGetPr(mx)[0] = (*fs).status;
   mxSetField(struct_array_ptr,0,"status",mx);

   mx = mxCreateDoubleMatrix(1,1,mxREAL);
   mxGetPr(mx)[0] = (*fs).input;
   mxSetField(struct_array_ptr,0,"input",mx);

   mx = mxCreateDoubleMatrix(1,1,mxREAL);
   mxGetPr(mx)[0] = (*fs).buttons;
   mxSetField(struct_array_ptr,0,"buttons",mx);

   mx = mxCreateDoubleMatrix(1,1,mxREAL);
   mxGetPr(mx)[0] = (*fs).htype;
   mxSetField(struct_array_ptr,0,"htype",mx);
           
   mx = mxCreateDoubleMatrix(1,8,mxREAL);
   mxGetPr(mx)[0] = (*fs).hdata[0];
   mxGetPr(mx)[1] = (*fs).hdata[1];
   mxGetPr(mx)[2] = (*fs).hdata[2];
   mxGetPr(mx)[3] = (*fs).hdata[3];
   mxGetPr(mx)[4] = (*fs).hdata[4];
   mxGetPr(mx)[5] = (*fs).hdata[5];
   mxGetPr(mx)[6] = (*fs).hdata[6];
   mxGetPr(mx)[7] = (*fs).hdata[7];         
   mxSetField(struct_array_ptr,0,"hdata",mx);    
              
   return struct_array_ptr;
}

/*
ROUTINE: CreateMXFSample2
PURPOSE:
   copys a FSAMPLE2 structure to a matlab FSAMPLE structure
*/
mxArray *CreateMXFSample2(const FSAMPLE2 *fs2)
{
   const char *fieldNames[] = {"time","type","flags","px","py","hx","hy","pa","gx","gy",
                               "rx","ry","status","input","buttons","htype","hdata",
                               "eyeInHeadX", "eyeInHeadY",
                               "headMarkerFlags", "headRotation", "headPosition", "headInSpaceX", "headInSpaceY", 
                               "unused_headInSpaceX", "unused_headInSpaceY", 
                               "hrgx", "hrgy", "ergx", "ergy",
                               "ex", "ey", "emajor", "eminor", "eangle",
                               "unused"};
   const int fieldCount=sizeof(fieldNames)/sizeof(*fieldNames);   
   mxArray *struct_array_ptr, *mx;
 
   if ((*fs2).type != SAMPLE_TYPE){
     // PrintfExit("CreateMXFSample2: wrong pointer argument\n");
     printf("CreateMXFSample2: wrong pointer argument\n");
	 return(NULL);
}

   /* Create a 1-by-1 structmatrix. */  
   struct_array_ptr = mxCreateStructMatrix(1,1,fieldCount,fieldNames);   
   if (struct_array_ptr == NULL)
      PrintfExit("Could not create struct matrix (probably out of memory)\n");

   mx = mxCreateDoubleMatrix(1,1,mxREAL);
   mxGetPr(mx)[0] = FLOAT_TIME(fs2); //backwards incompatible change from (*fs).time -- with new 2kHz sample rates, time (in ms) can be fractional, this macro checks fs2->flags to see if the uint32 fs2->time needs an extra .5 ms
   mxSetField(struct_array_ptr,0,"time",mx);

   mx = mxCreateDoubleMatrix(1,1,mxREAL);
   mxGetPr(mx)[0] = (*fs2).type;
   mxSetField(struct_array_ptr,0,"type",mx);

   mx = mxCreateDoubleMatrix(1,1,mxREAL);
   mxGetPr(mx)[0] = (*fs2).flags;
   mxSetField(struct_array_ptr,0,"flags",mx);
   
   mx = mxCreateDoubleMatrix(1,2,mxREAL);
   mxGetPr(mx)[0] = (*fs2).px[0];
   mxGetPr(mx)[1] = (*fs2).px[1];
   mxSetField(struct_array_ptr,0,"px",mx);
      
   mx = mxCreateDoubleMatrix(1,2,mxREAL);
   mxGetPr(mx)[0] = (*fs2).py[0];
   mxGetPr(mx)[1] = (*fs2).py[1];
   mxSetField(struct_array_ptr,0,"py",mx);
   
   mx = mxCreateDoubleMatrix(1,2,mxREAL);
   mxGetPr(mx)[0] = (*fs2).hx[0];
   mxGetPr(mx)[1] = (*fs2).hx[1];
   mxSetField(struct_array_ptr,0,"hx",mx);
         
   mx = mxCreateDoubleMatrix(1,2,mxREAL);
   mxGetPr(mx)[0] = (*fs2).hy[0];
   mxGetPr(mx)[1] = (*fs2).hy[1];
   mxSetField(struct_array_ptr,0,"hy",mx); 

   mx = mxCreateDoubleMatrix(1,2,mxREAL);
   mxGetPr(mx)[0] = (*fs2).pa[0];
   mxGetPr(mx)[1] = (*fs2).pa[1];
   mxSetField(struct_array_ptr,0,"pa",mx);    
   
   mx = mxCreateDoubleMatrix(1,2,mxREAL);
   mxGetPr(mx)[0] = (*fs2).gx[0];
   mxGetPr(mx)[1] = (*fs2).gx[1];
   mxSetField(struct_array_ptr,0,"gx",mx);    
   
   mx = mxCreateDoubleMatrix(1,2,mxREAL);
   mxGetPr(mx)[0] = (*fs2).gy[0];
   mxGetPr(mx)[1] = (*fs2).gy[1];
   mxSetField(struct_array_ptr,0,"gy",mx);

   mx = mxCreateDoubleMatrix(1,1,mxREAL);
   mxGetPr(mx)[0] = (*fs2).rx;
   mxSetField(struct_array_ptr,0,"rx",mx);

   mx = mxCreateDoubleMatrix(1,1,mxREAL);
   mxGetPr(mx)[0] = (*fs2).ry;
   mxSetField(struct_array_ptr,0,"ry",mx);

   mx = mxCreateDoubleMatrix(1,1,mxREAL);
   mxGetPr(mx)[0] = (*fs2).status;
   mxSetField(struct_array_ptr,0,"status",mx);

   mx = mxCreateDoubleMatrix(1,1,mxREAL);
   mxGetPr(mx)[0] = (*fs2).input;
   mxSetField(struct_array_ptr,0,"input",mx);

   mx = mxCreateDoubleMatrix(1,1,mxREAL);
   mxGetPr(mx)[0] = (*fs2).buttons;
   mxSetField(struct_array_ptr,0,"buttons",mx);

   mx = mxCreateDoubleMatrix(1,1,mxREAL);
   mxGetPr(mx)[0] = (*fs2).htype;
   mxSetField(struct_array_ptr,0,"htype",mx);
           
   mx = mxCreateDoubleMatrix(1,8,mxREAL);
   mxGetPr(mx)[0] = (*fs2).hdata[0];
   mxGetPr(mx)[1] = (*fs2).hdata[1];
   mxGetPr(mx)[2] = (*fs2).hdata[2];
   mxGetPr(mx)[3] = (*fs2).hdata[3];
   mxGetPr(mx)[4] = (*fs2).hdata[4];
   mxGetPr(mx)[5] = (*fs2).hdata[5];
   mxGetPr(mx)[6] = (*fs2).hdata[6];
   mxGetPr(mx)[7] = (*fs2).hdata[7];
   mxSetField(struct_array_ptr,0,"hdata",mx);

   mx = mxCreateDoubleMatrix(1,3,mxREAL);
   mxGetPr(mx)[0] = (*fs2).eyeInHeadX[0];
   mxGetPr(mx)[1] = (*fs2).eyeInHeadX[1];
   mxGetPr(mx)[2] = (*fs2).eyeInHeadX[2];
   mxSetField(struct_array_ptr,0,"eyeInHeadX",mx);

   mx = mxCreateDoubleMatrix(1,3,mxREAL);
   mxGetPr(mx)[0] = (*fs2).eyeInHeadY[0];
   mxGetPr(mx)[1] = (*fs2).eyeInHeadY[1];
   mxGetPr(mx)[2] = (*fs2).eyeInHeadY[2];
   mxSetField(struct_array_ptr,0,"eyeInHeadY",mx);

   mx = mxCreateDoubleMatrix(1,1,mxREAL);
   mxGetPr(mx)[0] = (*fs2).headMarkerFlags;
   mxSetField(struct_array_ptr,0,"headMarkerFlags",mx);

   mx = mxCreateDoubleMatrix(1,3,mxREAL);
   mxGetPr(mx)[0] = (*fs2).headRotation[0];
   mxGetPr(mx)[1] = (*fs2).headRotation[1];
   mxGetPr(mx)[2] = (*fs2).headRotation[2];
   mxSetField(struct_array_ptr,0,"headRotation",mx);
   
   mx = mxCreateDoubleMatrix(1,3,mxREAL);
   mxGetPr(mx)[0] = (*fs2).headPosition[0];
   mxGetPr(mx)[1] = (*fs2).headPosition[1];
   mxGetPr(mx)[2] = (*fs2).headPosition[2];
   mxSetField(struct_array_ptr,0,"headPosition",mx);

   mx = mxCreateDoubleMatrix(1,1,mxREAL);
   mxGetPr(mx)[0] = (*fs2).headInSpaceX;
   mxSetField(struct_array_ptr,0,"headInSpaceX",mx);

   mx = mxCreateDoubleMatrix(1,1,mxREAL);
   mxGetPr(mx)[0] = (*fs2).headInSpaceY;
   mxSetField(struct_array_ptr,0,"headInSpaceY",mx);

   mx = mxCreateDoubleMatrix(1,2,mxREAL);
   mxGetPr(mx)[0] = (*fs2).unused_headInSpaceX[0];
   mxGetPr(mx)[1] = (*fs2).unused_headInSpaceX[1];
   mxSetField(struct_array_ptr,0,"unused_headInSpaceX",mx);

   mx = mxCreateDoubleMatrix(1,2,mxREAL);
   mxGetPr(mx)[0] = (*fs2).unused_headInSpaceY[0];
   mxGetPr(mx)[1] = (*fs2).unused_headInSpaceY[1];
   mxSetField(struct_array_ptr,0,"unused_headInSpaceY",mx);

   mx = mxCreateDoubleMatrix(1,1,mxREAL);
   mxGetPr(mx)[0] = (*fs2).hrgx;
   mxSetField(struct_array_ptr,0,"hrgx",mx);

   mx = mxCreateDoubleMatrix(1,1,mxREAL);
   mxGetPr(mx)[0] = (*fs2).hrgy;
   mxSetField(struct_array_ptr,0,"hrgy",mx);

   mx = mxCreateDoubleMatrix(1,1,mxREAL);
   mxGetPr(mx)[0] = (*fs2).ergx;
   mxSetField(struct_array_ptr,0,"ergx",mx);

   mx = mxCreateDoubleMatrix(1,1,mxREAL);
   mxGetPr(mx)[0] = (*fs2).ergy;
   mxSetField(struct_array_ptr,0,"ergy",mx);

   mx = mxCreateDoubleMatrix(1,2,mxREAL);
   mxGetPr(mx)[0] = (*fs2).ex[0];
   mxGetPr(mx)[1] = (*fs2).ex[1];
   mxSetField(struct_array_ptr,0,"ex",mx);

   mx = mxCreateDoubleMatrix(1,2,mxREAL);
   mxGetPr(mx)[0] = (*fs2).ey[0];
   mxGetPr(mx)[1] = (*fs2).ey[1];
   mxSetField(struct_array_ptr,0,"ey",mx);

   mx = mxCreateDoubleMatrix(1,2,mxREAL);
   mxGetPr(mx)[0] = (*fs2).emajor[0];
   mxGetPr(mx)[1] = (*fs2).emajor[1];
   mxSetField(struct_array_ptr,0,"emajor",mx);

   mx = mxCreateDoubleMatrix(1,2,mxREAL);
   mxGetPr(mx)[0] = (*fs2).eminor[0];
   mxGetPr(mx)[1] = (*fs2).eminor[1];
   mxSetField(struct_array_ptr,0,"eminor",mx);

   mx = mxCreateDoubleMatrix(1,2,mxREAL);
   mxGetPr(mx)[0] = (*fs2).eangle[0];
   mxGetPr(mx)[1] = (*fs2).eangle[1];
   mxSetField(struct_array_ptr,0,"eangle",mx);

   mx = mxCreateDoubleMatrix(1,20,mxREAL);
   mxGetPr(mx)[0] = (*fs2).unused[0];
   mxGetPr(mx)[1] = (*fs2).unused[1];
   mxGetPr(mx)[2] = (*fs2).unused[2];
   mxGetPr(mx)[3] = (*fs2).unused[3];
   mxGetPr(mx)[4] = (*fs2).unused[4];
   mxGetPr(mx)[5] = (*fs2).unused[5];
   mxGetPr(mx)[6] = (*fs2).unused[6];
   mxGetPr(mx)[7] = (*fs2).unused[7];
   mxGetPr(mx)[8] = (*fs2).unused[8];
   mxGetPr(mx)[9] = (*fs2).unused[9];
   mxGetPr(mx)[10] = (*fs2).unused[10];
   mxGetPr(mx)[11] = (*fs2).unused[11];
   mxGetPr(mx)[12] = (*fs2).unused[12];
   mxGetPr(mx)[13] = (*fs2).unused[13];
   mxGetPr(mx)[14] = (*fs2).unused[14];
   mxGetPr(mx)[15] = (*fs2).unused[15];
   mxGetPr(mx)[16] = (*fs2).unused[16];
   mxGetPr(mx)[17] = (*fs2).unused[17];
   mxGetPr(mx)[18] = (*fs2).unused[18];
   mxGetPr(mx)[19] = (*fs2).unused[19];
   mxSetField(struct_array_ptr,0,"unused",mx);

   return struct_array_ptr;
}

/* RAW SAMPLE STRUCT */

mxArray *CreateMXFSampleRaw(const FSAMPLE_RAW *fs)
{
   const char *fieldNames[] = {"raw_pupil","raw_cr","pupil_area","cr_area","pupil_dimension",
								"cr_dimension", "window_position","pupil_cr","cr_area2","raw_cr2"};
   const int fieldCount=sizeof(fieldNames)/sizeof(*fieldNames);   
   mxArray *struct_array_ptr, *mx;
 
   /* Create a 1-by-1 structmatrix. */  
   struct_array_ptr = mxCreateStructMatrix(1,1,fieldCount,fieldNames);   
   if (struct_array_ptr == NULL)
      PrintfExit("Could not create struct matrix (probably out of memory)\n");

   mx = mxCreateDoubleMatrix(1,2,mxREAL);
   mxGetPr(mx)[0] = (*fs).raw_pupil[0];
   mxGetPr(mx)[1] = (*fs).raw_pupil[1];
   mxSetField(struct_array_ptr,0,"raw_pupil",mx);

   mx = mxCreateDoubleMatrix(1,2,mxREAL);
   mxGetPr(mx)[0] = (*fs).raw_cr[0];
   mxGetPr(mx)[1] = (*fs).raw_cr[1];
   mxSetField(struct_array_ptr,0,"raw_cr",mx);

   mx = mxCreateDoubleMatrix(1,1,mxREAL);
   mxGetPr(mx)[0] = (*fs).cr_area;
   mxSetField(struct_array_ptr,0,"cr_area",mx);

   mx = mxCreateDoubleMatrix(1,1,mxREAL);
   mxGetPr(mx)[0] = (*fs).pupil_area;
   mxSetField(struct_array_ptr,0,"pupil_area",mx);

   mx = mxCreateDoubleMatrix(1,2,mxREAL);
   mxGetPr(mx)[0] = (*fs).pupil_dimension[0];
   mxGetPr(mx)[1] = (*fs).pupil_dimension[1];
   mxSetField(struct_array_ptr,0,"pupil_dimension",mx);

   mx = mxCreateDoubleMatrix(1,2,mxREAL);
   mxGetPr(mx)[0] = (*fs).cr_dimension[0];
   mxGetPr(mx)[1] = (*fs).cr_dimension[1];
   mxSetField(struct_array_ptr,0,"cr_dimension",mx);

   mx = mxCreateDoubleMatrix(1,2,mxREAL);
   mxGetPr(mx)[0] = (*fs).window_position[0];
   mxGetPr(mx)[1] = (*fs).window_position[1];
   mxSetField(struct_array_ptr,0,"window_position",mx);

   mx = mxCreateDoubleMatrix(1,2,mxREAL);
   mxGetPr(mx)[0] = (*fs).pupil_cr[0];
   mxGetPr(mx)[1] = (*fs).pupil_cr[1];
   mxSetField(struct_array_ptr,0,"pupil_cr",mx);
              
   mx = mxCreateDoubleMatrix(1,1,mxREAL);
   mxGetPr(mx)[0] = (*fs).cr_area2;
   mxSetField(struct_array_ptr,0,"cr_area2",mx);
   
   mx = mxCreateDoubleMatrix(1,2,mxREAL);
   mxGetPr(mx)[0] = (*fs).raw_cr2[0];
   mxGetPr(mx)[1] = (*fs).raw_cr2[1];
   mxSetField(struct_array_ptr,0,"raw_cr2",mx);
   
   return struct_array_ptr;
}

/*
ROUTINE: CreateMXFEvent
PURPOSE:
   copys a FEVENT structure to a matlab FEVENT structure*/   

		
mxArray *CreateMXFEvent(const FEVENT *fe)
{
   const char *fieldNames[] = {"time","type","read","eye","sttime","entime","hstx","hsty",
                               "gstx","gsty","sta","henx","heny","genx","geny",
                               "ena","havx","havy","gavx","gavy","ava","avel","pvel",
                               "svel","evel","supd_x","eupd_x","supd_y","eupd_y","status",
                               };
                               
   const int fieldCount=sizeof(fieldNames)/sizeof(*fieldNames);   
   mxArray *struct_array_ptr, *mx; 
   
   if ((*fe).type == SAMPLE_TYPE)
      PrintfExit("CreateMXFEvent : wrong pointer argument\n");
 
   /* Create a 1-by-1 structmatrix. */  
   struct_array_ptr = mxCreateStructMatrix(1,1,fieldCount,fieldNames);   
   if (struct_array_ptr == NULL)
      PrintfExit("Could not create struct matrix (probably out of memory)\n");

   mx = mxCreateDoubleMatrix(1,1,mxREAL);
   mxGetPr(mx)[0] = (*fe).time; // FLOAT_TIME currently a noop on events, but may change in future!
   mxSetField(struct_array_ptr,0,"time",mx);

   mx = mxCreateDoubleMatrix(1,1,mxREAL);
   mxGetPr(mx)[0] = (*fe).type;
   mxSetField(struct_array_ptr,0,"type",mx);
   
   mx = mxCreateDoubleMatrix(1,1,mxREAL);
   mxGetPr(mx)[0] = (*fe).read;
   mxSetField(struct_array_ptr,0,"read",mx);

   mx = mxCreateDoubleMatrix(1,1,mxREAL);
   mxGetPr(mx)[0] = (*fe).eye;
   mxSetField(struct_array_ptr,0,"eye",mx);  
   
   mx = mxCreateDoubleMatrix(1,1,mxREAL);
   mxGetPr(mx)[0] = (*fe).sttime;
   mxSetField(struct_array_ptr,0,"sttime",mx);

   mx = mxCreateDoubleMatrix(1,1,mxREAL);
   mxGetPr(mx)[0] = (*fe).entime;
   mxSetField(struct_array_ptr,0,"entime",mx);     
   
   mx = mxCreateDoubleMatrix(1,1,mxREAL);
   mxGetPr(mx)[0] = (*fe).hstx;
   mxSetField(struct_array_ptr,0,"hstx",mx);

   mx = mxCreateDoubleMatrix(1,1,mxREAL);
   mxGetPr(mx)[0] = (*fe).hsty;
   mxSetField(struct_array_ptr,0,"hsty",mx);       

   mx = mxCreateDoubleMatrix(1,1,mxREAL);
   mxGetPr(mx)[0] = (*fe).gstx;
   mxSetField(struct_array_ptr,0,"gstx",mx);

   mx = mxCreateDoubleMatrix(1,1,mxREAL);
   mxGetPr(mx)[0] = (*fe).gsty;
   mxSetField(struct_array_ptr,0,"gsty",mx);
   
   mx = mxCreateDoubleMatrix(1,1,mxREAL);
   mxGetPr(mx)[0] = (*fe).sta;
   mxSetField(struct_array_ptr,0,"sta",mx);

   mx = mxCreateDoubleMatrix(1,1,mxREAL);
   mxGetPr(mx)[0] = (*fe).henx;
   mxSetField(struct_array_ptr,0,"henx",mx);
   
   mx = mxCreateDoubleMatrix(1,1,mxREAL);
   mxGetPr(mx)[0] = (*fe).heny;
   mxSetField(struct_array_ptr,0,"heny",mx);
   
   mx = mxCreateDoubleMatrix(1,1,mxREAL);
   mxGetPr(mx)[0] = (*fe).genx;
   mxSetField(struct_array_ptr,0,"genx",mx);
   
   mx = mxCreateDoubleMatrix(1,1,mxREAL);
   mxGetPr(mx)[0] = (*fe).geny;
   mxSetField(struct_array_ptr,0,"geny",mx);
   
   mx = mxCreateDoubleMatrix(1,1,mxREAL);
   mxGetPr(mx)[0] = (*fe).ena;
   mxSetField(struct_array_ptr,0,"ena",mx);
   
   mx = mxCreateDoubleMatrix(1,1,mxREAL);
   mxGetPr(mx)[0] = (*fe).havx;
   mxSetField(struct_array_ptr,0,"havx",mx);
   
   mx = mxCreateDoubleMatrix(1,1,mxREAL);
   mxGetPr(mx)[0] = (*fe).havy;
   mxSetField(struct_array_ptr,0,"havy",mx);
   
   mx = mxCreateDoubleMatrix(1,1,mxREAL);
   mxGetPr(mx)[0] = (*fe).gavx;
   mxSetField(struct_array_ptr,0,"gavx",mx);
   
   mx = mxCreateDoubleMatrix(1,1,mxREAL);
   mxGetPr(mx)[0] = (*fe).gavy;
   mxSetField(struct_array_ptr,0,"gavy",mx);   
   
   mx = mxCreateDoubleMatrix(1,1,mxREAL);
   mxGetPr(mx)[0] = (*fe).ava;
   mxSetField(struct_array_ptr,0,"ava",mx);
   
   mx = mxCreateDoubleMatrix(1,1,mxREAL);
   mxGetPr(mx)[0] = (*fe).avel;
   mxSetField(struct_array_ptr,0,"avel",mx);
   
   mx = mxCreateDoubleMatrix(1,1,mxREAL);
   mxGetPr(mx)[0] = (*fe).pvel;
   mxSetField(struct_array_ptr,0,"pvel",mx); 
   
   mx = mxCreateDoubleMatrix(1,1,mxREAL);
   mxGetPr(mx)[0] = (*fe).svel;
   mxSetField(struct_array_ptr,0,"svel",mx);
   
   mx = mxCreateDoubleMatrix(1,1,mxREAL);
   mxGetPr(mx)[0] = (*fe).evel;
   mxSetField(struct_array_ptr,0,"evel",mx);
   
   mx = mxCreateDoubleMatrix(1,1,mxREAL);
   mxGetPr(mx)[0] = (*fe).supd_x;
   mxSetField(struct_array_ptr,0,"supd_x",mx);
   
   mx = mxCreateDoubleMatrix(1,1,mxREAL);
   mxGetPr(mx)[0] = (*fe).eupd_x;
   mxSetField(struct_array_ptr,0,"eupd_x",mx);
   
   mx = mxCreateDoubleMatrix(1,1,mxREAL);
   mxGetPr(mx)[0] = (*fe).supd_y;
   mxSetField(struct_array_ptr,0,"supd_y",mx);
   
   mx = mxCreateDoubleMatrix(1,1,mxREAL);
   mxGetPr(mx)[0] = (*fe).eupd_y;
   mxSetField(struct_array_ptr,0,"eupd_y",mx);
   
   mx = mxCreateDoubleMatrix(1,1,mxREAL);
   mxGetPr(mx)[0] = (*fe).status;
   mxSetField(struct_array_ptr,0,"status",mx);
   
   return struct_array_ptr;
}

/*
ROUTINE: CreateMXFEvent2
PURPOSE:
   copys a FEVENT structure to a matlab FEVENT structure
*/   

		
mxArray *CreateMXFEvent2(const FEVENT2 *fe2)
{
   const char *fieldNames[] = {"time","type","read","eye","sttime","entime","hstx","hsty",
                               "gstx","gsty","sta","henx","heny","genx","geny",
                               "ena","havx","havy","gavx","gavy","ava","avel","pvel",
                               "svel","evel","supd_x","eupd_x","supd_y","eupd_y","status",
                               "headInSpaceX_start","headInSpaceY_start","headInSpaceX_end","headInSpaceY_end","headInSpaceX_avg","headInSpaceY_avg",
                               "eyeInHeadX_start","eyeInHeadY_start","eyeInHeadX_end","eyeInHeadY_end","eyeInHeadX_avg","eyeInHeadY_avg",
                               "headRotationX_start","headRotationY_start","headRotationZ_start",
                               "headRotationX_end","headRotationY_end","headRotationZ_end",
                               "headRotationX_avg","headRotationY_avg","headRotationZ_avg",
                               "headPositionX_start","headPositionY_start","headPositionZ_start",
                               "headPositionX_end","headPositionY_end","headPositionZ_end",
                               "headPositionX_avg","headPositionY_avg","headPositionZ_avg",
                               "unused"};
   const int fieldCount=sizeof(fieldNames)/sizeof(*fieldNames);   
   mxArray *struct_array_ptr, *mx; 
   
   if ((*fe2).type == SAMPLE_TYPE)
      PrintfExit("CreateMXFEvent : wrong pointer argument\n");
 
   /* Create a 1-by-1 structmatrix. */  
   struct_array_ptr = mxCreateStructMatrix(1,1,fieldCount,fieldNames);   
   if (struct_array_ptr == NULL)
      PrintfExit("Could not create struct matrix (probably out of memory)\n");

   mx = mxCreateDoubleMatrix(1,1,mxREAL);
   mxGetPr(mx)[0] = (*fe2).time; // FLOAT_TIME currently a noop on events, but may change in future!
   mxSetField(struct_array_ptr,0,"time",mx);

   mx = mxCreateDoubleMatrix(1,1,mxREAL);
   mxGetPr(mx)[0] = (*fe2).type;
   mxSetField(struct_array_ptr,0,"type",mx);
   
   mx = mxCreateDoubleMatrix(1,1,mxREAL);
   mxGetPr(mx)[0] = (*fe2).read;
   mxSetField(struct_array_ptr,0,"read",mx);

   mx = mxCreateDoubleMatrix(1,1,mxREAL);
   mxGetPr(mx)[0] = (*fe2).eye;
   mxSetField(struct_array_ptr,0,"eye",mx);  
   
   mx = mxCreateDoubleMatrix(1,1,mxREAL);
   mxGetPr(mx)[0] = (*fe2).sttime;
   mxSetField(struct_array_ptr,0,"sttime",mx);

   mx = mxCreateDoubleMatrix(1,1,mxREAL);
   mxGetPr(mx)[0] = (*fe2).entime;
   mxSetField(struct_array_ptr,0,"entime",mx);     
   
   mx = mxCreateDoubleMatrix(1,1,mxREAL);
   mxGetPr(mx)[0] = (*fe2).hstx;
   mxSetField(struct_array_ptr,0,"hstx",mx);

   mx = mxCreateDoubleMatrix(1,1,mxREAL);
   mxGetPr(mx)[0] = (*fe2).hsty;
   mxSetField(struct_array_ptr,0,"hsty",mx);       

   mx = mxCreateDoubleMatrix(1,1,mxREAL);
   mxGetPr(mx)[0] = (*fe2).gstx;
   mxSetField(struct_array_ptr,0,"gstx",mx);

   mx = mxCreateDoubleMatrix(1,1,mxREAL);
   mxGetPr(mx)[0] = (*fe2).gsty;
   mxSetField(struct_array_ptr,0,"gsty",mx);
   
   mx = mxCreateDoubleMatrix(1,1,mxREAL);
   mxGetPr(mx)[0] = (*fe2).sta;
   mxSetField(struct_array_ptr,0,"sta",mx);

   mx = mxCreateDoubleMatrix(1,1,mxREAL);
   mxGetPr(mx)[0] = (*fe2).henx;
   mxSetField(struct_array_ptr,0,"henx",mx);
   
   mx = mxCreateDoubleMatrix(1,1,mxREAL);
   mxGetPr(mx)[0] = (*fe2).heny;
   mxSetField(struct_array_ptr,0,"heny",mx);
   
   mx = mxCreateDoubleMatrix(1,1,mxREAL);
   mxGetPr(mx)[0] = (*fe2).genx;
   mxSetField(struct_array_ptr,0,"genx",mx);
   
   mx = mxCreateDoubleMatrix(1,1,mxREAL);
   mxGetPr(mx)[0] = (*fe2).geny;
   mxSetField(struct_array_ptr,0,"geny",mx);
   
   mx = mxCreateDoubleMatrix(1,1,mxREAL);
   mxGetPr(mx)[0] = (*fe2).ena;
   mxSetField(struct_array_ptr,0,"ena",mx);
   
   mx = mxCreateDoubleMatrix(1,1,mxREAL);
   mxGetPr(mx)[0] = (*fe2).havx;
   mxSetField(struct_array_ptr,0,"havx",mx);
   
   mx = mxCreateDoubleMatrix(1,1,mxREAL);
   mxGetPr(mx)[0] = (*fe2).havy;
   mxSetField(struct_array_ptr,0,"havy",mx);
   
   mx = mxCreateDoubleMatrix(1,1,mxREAL);
   mxGetPr(mx)[0] = (*fe2).gavx;
   mxSetField(struct_array_ptr,0,"gavx",mx);
   
   mx = mxCreateDoubleMatrix(1,1,mxREAL);
   mxGetPr(mx)[0] = (*fe2).gavy;
   mxSetField(struct_array_ptr,0,"gavy",mx);   
   
   mx = mxCreateDoubleMatrix(1,1,mxREAL);
   mxGetPr(mx)[0] = (*fe2).ava;
   mxSetField(struct_array_ptr,0,"ava",mx);
   
   mx = mxCreateDoubleMatrix(1,1,mxREAL);
   mxGetPr(mx)[0] = (*fe2).avel;
   mxSetField(struct_array_ptr,0,"avel",mx);
   
   mx = mxCreateDoubleMatrix(1,1,mxREAL);
   mxGetPr(mx)[0] = (*fe2).pvel;
   mxSetField(struct_array_ptr,0,"pvel",mx); 
   
   mx = mxCreateDoubleMatrix(1,1,mxREAL);
   mxGetPr(mx)[0] = (*fe2).svel;
   mxSetField(struct_array_ptr,0,"svel",mx);
   
   mx = mxCreateDoubleMatrix(1,1,mxREAL);
   mxGetPr(mx)[0] = (*fe2).evel;
   mxSetField(struct_array_ptr,0,"evel",mx);
   
   mx = mxCreateDoubleMatrix(1,1,mxREAL);
   mxGetPr(mx)[0] = (*fe2).supd_x;
   mxSetField(struct_array_ptr,0,"supd_x",mx);
   
   mx = mxCreateDoubleMatrix(1,1,mxREAL);
   mxGetPr(mx)[0] = (*fe2).eupd_x;
   mxSetField(struct_array_ptr,0,"eupd_x",mx);
   
   mx = mxCreateDoubleMatrix(1,1,mxREAL);
   mxGetPr(mx)[0] = (*fe2).supd_y;
   mxSetField(struct_array_ptr,0,"supd_y",mx);
   
   mx = mxCreateDoubleMatrix(1,1,mxREAL);
   mxGetPr(mx)[0] = (*fe2).eupd_y;
   mxSetField(struct_array_ptr,0,"eupd_y",mx);
   
   mx = mxCreateDoubleMatrix(1,1,mxREAL);
   mxGetPr(mx)[0] = (*fe2).status;
   mxSetField(struct_array_ptr,0,"status",mx);

   mx = mxCreateDoubleMatrix(1,1,mxREAL);
   mxGetPr(mx)[0] = (*fe2).headInSpaceX_start;
   mxSetField(struct_array_ptr,0,"headInSpaceX_start",mx);

   mx = mxCreateDoubleMatrix(1,1,mxREAL);
   mxGetPr(mx)[0] = (*fe2).headInSpaceY_start;
   mxSetField(struct_array_ptr,0,"headInSpaceY_start",mx);

   mx = mxCreateDoubleMatrix(1,1,mxREAL);
   mxGetPr(mx)[0] = (*fe2).headInSpaceX_end;
   mxSetField(struct_array_ptr,0,"headInSpaceX_end",mx);

   mx = mxCreateDoubleMatrix(1,1,mxREAL);
   mxGetPr(mx)[0] = (*fe2).headInSpaceY_end;
   mxSetField(struct_array_ptr,0,"headInSpaceY_end",mx);

   mx = mxCreateDoubleMatrix(1,1,mxREAL);
   mxGetPr(mx)[0] = (*fe2).headInSpaceX_avg;
   mxSetField(struct_array_ptr,0,"headInSpaceX_avg",mx);

   mx = mxCreateDoubleMatrix(1,1,mxREAL);
   mxGetPr(mx)[0] = (*fe2).headInSpaceY_avg;
   mxSetField(struct_array_ptr,0,"headInSpaceY_avg",mx);

   mx = mxCreateDoubleMatrix(1,1,mxREAL);
   mxGetPr(mx)[0] = (*fe2).eyeInHeadX_start;
   mxSetField(struct_array_ptr,0,"eyeInHeadX_start",mx);

   mx = mxCreateDoubleMatrix(1,1,mxREAL);
   mxGetPr(mx)[0] = (*fe2).eyeInHeadY_start;
   mxSetField(struct_array_ptr,0,"eyeInHeadY_start",mx);

   mx = mxCreateDoubleMatrix(1,1,mxREAL);
   mxGetPr(mx)[0] = (*fe2).eyeInHeadX_end;
   mxSetField(struct_array_ptr,0,"eyeInHeadX_end",mx);

   mx = mxCreateDoubleMatrix(1,1,mxREAL);
   mxGetPr(mx)[0] = (*fe2).eyeInHeadY_end;
   mxSetField(struct_array_ptr,0,"eyeInHeadY_end",mx);

   mx = mxCreateDoubleMatrix(1,1,mxREAL);
   mxGetPr(mx)[0] = (*fe2).eyeInHeadX_avg;
   mxSetField(struct_array_ptr,0,"eyeInHeadX_avg",mx);

   mx = mxCreateDoubleMatrix(1,1,mxREAL);
   mxGetPr(mx)[0] = (*fe2).eyeInHeadY_avg;
   mxSetField(struct_array_ptr,0,"eyeInHeadY_avg",mx);

   mx = mxCreateDoubleMatrix(1,1,mxREAL);
   mxGetPr(mx)[0] = (*fe2).headRotationX_start;
   mxSetField(struct_array_ptr,0,"headRotationX_start",mx);

   mx = mxCreateDoubleMatrix(1,1,mxREAL);
   mxGetPr(mx)[0] = (*fe2).headRotationY_start;
   mxSetField(struct_array_ptr,0,"headRotationY_start",mx);

   mx = mxCreateDoubleMatrix(1,1,mxREAL);
   mxGetPr(mx)[0] = (*fe2).headRotationZ_start;
   mxSetField(struct_array_ptr,0,"headRotationZ_start",mx);

   mx = mxCreateDoubleMatrix(1,1,mxREAL);
   mxGetPr(mx)[0] = (*fe2).headRotationX_end;
   mxSetField(struct_array_ptr,0,"headRotationX_end",mx);

   mx = mxCreateDoubleMatrix(1,1,mxREAL);
   mxGetPr(mx)[0] = (*fe2).headRotationY_end;
   mxSetField(struct_array_ptr,0,"headRotationY_end",mx);

   mx = mxCreateDoubleMatrix(1,1,mxREAL);
   mxGetPr(mx)[0] = (*fe2).headRotationZ_end;
   mxSetField(struct_array_ptr,0,"headRotationZ_end",mx);

   mx = mxCreateDoubleMatrix(1,1,mxREAL);
   mxGetPr(mx)[0] = (*fe2).headRotationX_avg;
   mxSetField(struct_array_ptr,0,"headRotationX_avg",mx);

   mx = mxCreateDoubleMatrix(1,1,mxREAL);
   mxGetPr(mx)[0] = (*fe2).headRotationY_avg;
   mxSetField(struct_array_ptr,0,"headRotationY_avg",mx);

   mx = mxCreateDoubleMatrix(1,1,mxREAL);
   mxGetPr(mx)[0] = (*fe2).headRotationZ_avg;
   mxSetField(struct_array_ptr,0,"headRotationZ_avg",mx);

   mx = mxCreateDoubleMatrix(1,1,mxREAL);
   mxGetPr(mx)[0] = (*fe2).headPositionX_start;
   mxSetField(struct_array_ptr,0,"headPositionX_start",mx);

   mx = mxCreateDoubleMatrix(1,1,mxREAL);
   mxGetPr(mx)[0] = (*fe2).headPositionY_start;
   mxSetField(struct_array_ptr,0,"headPositionY_start",mx);

   mx = mxCreateDoubleMatrix(1,1,mxREAL);
   mxGetPr(mx)[0] = (*fe2).headPositionZ_start;
   mxSetField(struct_array_ptr,0,"headPositionZ_start",mx);

   mx = mxCreateDoubleMatrix(1,1,mxREAL);
   mxGetPr(mx)[0] = (*fe2).headPositionX_end;
   mxSetField(struct_array_ptr,0,"headPositionX_end",mx);

   mx = mxCreateDoubleMatrix(1,1,mxREAL);
   mxGetPr(mx)[0] = (*fe2).headPositionY_end;
   mxSetField(struct_array_ptr,0,"headPositionY_end",mx);

   mx = mxCreateDoubleMatrix(1,1,mxREAL);
   mxGetPr(mx)[0] = (*fe2).headPositionZ_end;
   mxSetField(struct_array_ptr,0,"headPositionZ_end",mx);

   mx = mxCreateDoubleMatrix(1,1,mxREAL);
   mxGetPr(mx)[0] = (*fe2).headPositionX_avg;
   mxSetField(struct_array_ptr,0,"headPositionX_avg",mx);

   mx = mxCreateDoubleMatrix(1,1,mxREAL);
   mxGetPr(mx)[0] = (*fe2).headPositionY_avg;
   mxSetField(struct_array_ptr,0,"headPositionY_avg",mx);

   mx = mxCreateDoubleMatrix(1,1,mxREAL);
   mxGetPr(mx)[0] = (*fe2).headPositionZ_avg;
   mxSetField(struct_array_ptr,0,"headPositionZ_avg",mx);

   mx = mxCreateDoubleMatrix(1,16,mxREAL);
   mxGetPr(mx)[0] = (*fe2).unused[0];
   mxGetPr(mx)[1] = (*fe2).unused[1];
   mxGetPr(mx)[1] = (*fe2).unused[2];
   mxGetPr(mx)[1] = (*fe2).unused[3];
   mxGetPr(mx)[1] = (*fe2).unused[4];
   mxGetPr(mx)[1] = (*fe2).unused[5];
   mxGetPr(mx)[1] = (*fe2).unused[6];
   mxGetPr(mx)[1] = (*fe2).unused[7];
   mxGetPr(mx)[1] = (*fe2).unused[8];
   mxGetPr(mx)[1] = (*fe2).unused[9];
   mxGetPr(mx)[1] = (*fe2).unused[10];
   mxGetPr(mx)[1] = (*fe2).unused[11];
   mxGetPr(mx)[1] = (*fe2).unused[12];
   mxGetPr(mx)[1] = (*fe2).unused[13];
   mxGetPr(mx)[1] = (*fe2).unused[14];
   mxGetPr(mx)[1] = (*fe2).unused[15];
   mxSetField(struct_array_ptr,0,"unused",mx);

   return struct_array_ptr;
}

