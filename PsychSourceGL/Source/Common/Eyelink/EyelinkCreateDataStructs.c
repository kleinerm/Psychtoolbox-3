/*

	/osxptb/trunk/PsychSourceGL/Source/OSX/Eyelink/EyelinkCreateDataStructs.c
  
	PROJECTS: Eyelink 
  
	AUTHORS:
		cburns@berkeley.edu				cdb
		E.Peters@ai.rug.nl				emp
		f.w.cornelissen@med.rug.nl		fwc
  
	PLATFORMS:	Currently only OS X  
    
	HISTORY:
			2001	emp		created
		11/23/05    cdb		adapted for OSX.
		30/10/06	fwc		added CreateMXFSampleRaw

	TARGET LOCATION:

		Eyelink.mexmac resides in:
			EyelinkToolbox
			
*/
#include "PsychEyelink.h"

/*
ROUTINE: CreateMXFSample
PURPOSE:
   copys a FSAMPLE structure to a matlab FSAMPLE structure*/   

/* this function is very similar to CreateMXISample
   and actually contains exactly the
   same code, except that the argument is different (FSAMPLE * here).
   I needed to copy everything, because there is no way to check if
   a pointer is a FSAMPLE *, or a ISAMPLE *, and they contain exactly
   the same fields (only some types are different)
   it would be much nicer if there was only one function 
   which would return a SAMPLE structure. */
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
   mxGetPr(mx)[0] = (*fs).time;
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

/* RAW SAMPLE STRUCT */

mxArray *CreateMXFSampleRaw(const FSAMPLE_RAW *fs)
{
   const char *fieldNames[] = {"raw_pupil","raw_cr","pupil_area","cr_area","pupil_dimension",
								"cr_dimension", "window_position","pupil_cr"};
   const int fieldCount=sizeof(fieldNames)/sizeof(*fieldNames);   
   mxArray *struct_array_ptr, *mx;
   
//   if ((*fs).type != SAMPLE_TYPE)
////      PrintfExit("wrong pointer argument\n");
//      mexPrintf("wrong pointer argument\n");
 
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
              
   return struct_array_ptr;
}



/*
ROUTINE: CreateMXISample
PURPOSE:
   copys a ISAMPLE structure to a matlab ISAMPLE structure*/   

mxArray *CreateMXISample(const ISAMPLE *is)
{
   const char *fieldNames[] = {"time","type","flags","px","py","hx","hy","pa","gx","gy",
                               "rx","ry","status","input","buttons","htype","hdata"};
   const int fieldCount=sizeof(fieldNames)/sizeof(*fieldNames);   
   mxArray *struct_array_ptr, *mx; 
   
   if ((*is).type != SAMPLE_TYPE)
      PrintfExit("wrong pointer argument\n");
 
   /* Create a 1-by-1 structmatrix. */  
   struct_array_ptr = mxCreateStructMatrix(1,1,fieldCount,fieldNames);   
   if (struct_array_ptr == NULL)
      PrintfExit("Could not create struct matrix (probably out of memory)\n");

   mx = mxCreateDoubleMatrix(1,1,mxREAL);
   mxGetPr(mx)[0] = (*is).time;
   mxSetField(struct_array_ptr,0,"time",mx);

   mx = mxCreateDoubleMatrix(1,1,mxREAL);
   mxGetPr(mx)[0] = (*is).type;
   mxSetField(struct_array_ptr,0,"type",mx);

   mx = mxCreateDoubleMatrix(1,1,mxREAL);
   mxGetPr(mx)[0] = (*is).flags;
   mxSetField(struct_array_ptr,0,"flags",mx);
   
   mx = mxCreateDoubleMatrix(1,2,mxREAL);
   mxGetPr(mx)[0] = (*is).px[0];
   mxGetPr(mx)[1] = (*is).px[1];
   mxSetField(struct_array_ptr,0,"px",mx);
      
   mx = mxCreateDoubleMatrix(1,2,mxREAL);
   mxGetPr(mx)[0] = (*is).py[0];
   mxGetPr(mx)[1] = (*is).py[1];
   mxSetField(struct_array_ptr,0,"py",mx);
   
   mx = mxCreateDoubleMatrix(1,2,mxREAL);
   mxGetPr(mx)[0] = (*is).hx[0];
   mxGetPr(mx)[1] = (*is).hx[1];
   mxSetField(struct_array_ptr,0,"hx",mx);
         
   mx = mxCreateDoubleMatrix(1,2,mxREAL);
   mxGetPr(mx)[0] = (*is).hy[0];
   mxGetPr(mx)[1] = (*is).hy[1];
   mxSetField(struct_array_ptr,0,"hy",mx); 

   mx = mxCreateDoubleMatrix(1,2,mxREAL);
   mxGetPr(mx)[0] = (*is).pa[0];
   mxGetPr(mx)[1] = (*is).pa[1];
   mxSetField(struct_array_ptr,0,"pa",mx);    
   
   mx = mxCreateDoubleMatrix(1,2,mxREAL);
   mxGetPr(mx)[0] = (*is).gx[0];
   mxGetPr(mx)[1] = (*is).gx[1];
   mxSetField(struct_array_ptr,0,"gx",mx);    
   
   mx = mxCreateDoubleMatrix(1,2,mxREAL);
   mxGetPr(mx)[0] = (*is).gy[0];
   mxGetPr(mx)[1] = (*is).gy[1];
   mxSetField(struct_array_ptr,0,"gy",mx);

   mx = mxCreateDoubleMatrix(1,1,mxREAL);
   mxGetPr(mx)[0] = (*is).rx;
   mxSetField(struct_array_ptr,0,"rx",mx);

   mx = mxCreateDoubleMatrix(1,1,mxREAL);
   mxGetPr(mx)[0] = (*is).ry;
   mxSetField(struct_array_ptr,0,"ry",mx);

   mx = mxCreateDoubleMatrix(1,1,mxREAL);
   mxGetPr(mx)[0] = (*is).status;
   mxSetField(struct_array_ptr,0,"status",mx);

   mx = mxCreateDoubleMatrix(1,1,mxREAL);
   mxGetPr(mx)[0] = (*is).input;
   mxSetField(struct_array_ptr,0,"input",mx);

   mx = mxCreateDoubleMatrix(1,1,mxREAL);
   mxGetPr(mx)[0] = (*is).buttons;
   mxSetField(struct_array_ptr,0,"buttons",mx);

   mx = mxCreateDoubleMatrix(1,1,mxREAL);
   mxGetPr(mx)[0] = (*is).htype;
   mxSetField(struct_array_ptr,0,"htype",mx);
           
   mx = mxCreateDoubleMatrix(1,8,mxREAL);
   mxGetPr(mx)[0] = (*is).hdata[0];
   mxGetPr(mx)[1] = (*is).hdata[1];
   mxGetPr(mx)[2] = (*is).hdata[2];
   mxGetPr(mx)[3] = (*is).hdata[3];
   mxGetPr(mx)[4] = (*is).hdata[4];
   mxGetPr(mx)[5] = (*is).hdata[5];
   mxGetPr(mx)[6] = (*is).hdata[6];
   mxGetPr(mx)[7] = (*is).hdata[7];         
   mxSetField(struct_array_ptr,0,"hdata",mx);    
              
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
                               "svel","evel","supd_x","eupd_x","supd_y","eupd_y","status"};
   const int fieldCount=sizeof(fieldNames)/sizeof(*fieldNames);   
   mxArray *struct_array_ptr, *mx; 
   
   if ((*fe).type == SAMPLE_TYPE)
      PrintfExit("CreateMXFEvent : wrong pointer argument\n");
 
   /* Create a 1-by-1 structmatrix. */  
   struct_array_ptr = mxCreateStructMatrix(1,1,fieldCount,fieldNames);   
   if (struct_array_ptr == NULL)
      PrintfExit("Could not create struct matrix (probably out of memory)\n");

   mx = mxCreateDoubleMatrix(1,1,mxREAL);
   mxGetPr(mx)[0] = (*fe).time;
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
ROUTINE: CreateMXIEvent
PURPOSE:
   copys a IEVENT structure to a matlab IEVENT structure*/   

		
mxArray *CreateMXIEvent(const IEVENT *ie)
{
   const char *fieldNames[] = {"time","type","read","eye","sttime","entime","hstx","hsty",
                               "gstx","gsty","sta","henx","heny","genx","geny",
                               "ena","havx","havy","gavx","gavy","ava","avel","pvel",
                               "svel","evel","supd_x","eupd_x","supd_y","eupd_y","status"};
   const int fieldCount=sizeof(fieldNames)/sizeof(*fieldNames);   
   mxArray *struct_array_ptr, *mx; 
   
   if ((*ie).type == SAMPLE_TYPE)
      PrintfExit("CreateMXIEvent : wrong pointer argument\n");
 
   /* Create a 1-by-1 structmatrix. */  
   struct_array_ptr = mxCreateStructMatrix(1,1,fieldCount,fieldNames);   
   if (struct_array_ptr == NULL)
      PrintfExit("Could not create struct matrix (probably out of memory)\n");

   mx = mxCreateDoubleMatrix(1,1,mxREAL);
   mxGetPr(mx)[0] = (*ie).time;
   mxSetField(struct_array_ptr,0,"time",mx);

   mx = mxCreateDoubleMatrix(1,1,mxREAL);
   mxGetPr(mx)[0] = (*ie).type;
   mxSetField(struct_array_ptr,0,"type",mx);
   
   mx = mxCreateDoubleMatrix(1,1,mxREAL);
   mxGetPr(mx)[0] = (*ie).read;
   mxSetField(struct_array_ptr,0,"read",mx);

   mx = mxCreateDoubleMatrix(1,1,mxREAL);
   mxGetPr(mx)[0] = (*ie).eye;
   mxSetField(struct_array_ptr,0,"eye",mx);  
   
   mx = mxCreateDoubleMatrix(1,1,mxREAL);
   mxGetPr(mx)[0] = (*ie).sttime;
   mxSetField(struct_array_ptr,0,"sttime",mx);

   mx = mxCreateDoubleMatrix(1,1,mxREAL);
   mxGetPr(mx)[0] = (*ie).entime;
   mxSetField(struct_array_ptr,0,"entime",mx);     
   
   mx = mxCreateDoubleMatrix(1,1,mxREAL);
   mxGetPr(mx)[0] = (*ie).hstx;
   mxSetField(struct_array_ptr,0,"hstx",mx);

   mx = mxCreateDoubleMatrix(1,1,mxREAL);
   mxGetPr(mx)[0] = (*ie).hsty;
   mxSetField(struct_array_ptr,0,"hsty",mx);       

   mx = mxCreateDoubleMatrix(1,1,mxREAL);
   mxGetPr(mx)[0] = (*ie).gstx;
   mxSetField(struct_array_ptr,0,"gstx",mx);

   mx = mxCreateDoubleMatrix(1,1,mxREAL);
   mxGetPr(mx)[0] = (*ie).gsty;
   mxSetField(struct_array_ptr,0,"gsty",mx);
   
   mx = mxCreateDoubleMatrix(1,1,mxREAL);
   mxGetPr(mx)[0] = (*ie).sta;
   mxSetField(struct_array_ptr,0,"sta",mx);

   mx = mxCreateDoubleMatrix(1,1,mxREAL);
   mxGetPr(mx)[0] = (*ie).henx;
   mxSetField(struct_array_ptr,0,"henx",mx);
   
   mx = mxCreateDoubleMatrix(1,1,mxREAL);
   mxGetPr(mx)[0] = (*ie).heny;
   mxSetField(struct_array_ptr,0,"heny",mx);
   
   mx = mxCreateDoubleMatrix(1,1,mxREAL);
   mxGetPr(mx)[0] = (*ie).genx;
   mxSetField(struct_array_ptr,0,"genx",mx);
   
   mx = mxCreateDoubleMatrix(1,1,mxREAL);
   mxGetPr(mx)[0] = (*ie).geny;
   mxSetField(struct_array_ptr,0,"geny",mx);
   
   mx = mxCreateDoubleMatrix(1,1,mxREAL);
   mxGetPr(mx)[0] = (*ie).ena;
   mxSetField(struct_array_ptr,0,"ena",mx);
   
   mx = mxCreateDoubleMatrix(1,1,mxREAL);
   mxGetPr(mx)[0] = (*ie).havx;
   mxSetField(struct_array_ptr,0,"havx",mx);
   
   mx = mxCreateDoubleMatrix(1,1,mxREAL);
   mxGetPr(mx)[0] = (*ie).havy;
   mxSetField(struct_array_ptr,0,"havy",mx);
   
   mx = mxCreateDoubleMatrix(1,1,mxREAL);
   mxGetPr(mx)[0] = (*ie).gavx;
   mxSetField(struct_array_ptr,0,"gavx",mx);
   
   mx = mxCreateDoubleMatrix(1,1,mxREAL);
   mxGetPr(mx)[0] = (*ie).gavy;
   mxSetField(struct_array_ptr,0,"gavy",mx);   
   
   mx = mxCreateDoubleMatrix(1,1,mxREAL);
   mxGetPr(mx)[0] = (*ie).ava;
   mxSetField(struct_array_ptr,0,"ava",mx);
   
   mx = mxCreateDoubleMatrix(1,1,mxREAL);
   mxGetPr(mx)[0] = (*ie).avel;
   mxSetField(struct_array_ptr,0,"avel",mx);
   
   mx = mxCreateDoubleMatrix(1,1,mxREAL);
   mxGetPr(mx)[0] = (*ie).pvel;
   mxSetField(struct_array_ptr,0,"pvel",mx); 
   
   mx = mxCreateDoubleMatrix(1,1,mxREAL);
   mxGetPr(mx)[0] = (*ie).svel;
   mxSetField(struct_array_ptr,0,"svel",mx);
   
   mx = mxCreateDoubleMatrix(1,1,mxREAL);
   mxGetPr(mx)[0] = (*ie).evel;
   mxSetField(struct_array_ptr,0,"evel",mx);
   
   mx = mxCreateDoubleMatrix(1,1,mxREAL);
   mxGetPr(mx)[0] = (*ie).supd_x;
   mxSetField(struct_array_ptr,0,"supd_x",mx);
   
   mx = mxCreateDoubleMatrix(1,1,mxREAL);
   mxGetPr(mx)[0] = (*ie).eupd_x;
   mxSetField(struct_array_ptr,0,"eupd_x",mx);
   
   mx = mxCreateDoubleMatrix(1,1,mxREAL);
   mxGetPr(mx)[0] = (*ie).supd_y;
   mxSetField(struct_array_ptr,0,"supd_y",mx);
   
   mx = mxCreateDoubleMatrix(1,1,mxREAL);
   mxGetPr(mx)[0] = (*ie).eupd_y;
   mxSetField(struct_array_ptr,0,"eupd_y",mx);
   
   mx = mxCreateDoubleMatrix(1,1,mxREAL);
   mxGetPr(mx)[0] = (*ie).status;
   mxSetField(struct_array_ptr,0,"status",mx);
   
   return struct_array_ptr;
}
