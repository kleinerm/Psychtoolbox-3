/*
 *
 * cvEyeTracker is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * cvEyeTracker is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with cvEyeTracker; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *
 * cvEyeTracker - Version 1.2.5
 * Part of the openEyes ToolKit -- http://hcvl.hci.iastate.edu/openEyes
 * Release Date:
 * Authors : Dongheng Li <dhli@iastate.edu>
 *           Derrick Parkhurst <derrick.parkhurst@hcvl.hci.iastate.edu>
 *           Jason Babcock <babcock@nyu.edu>
 *           David Winfield <dwinfiel@iastate.edu>
 * Copyright (c) 2004-2006
 * All Rights Reserved.
 *
 */

#ifdef PSYCHCV_USE_OPENCV

// These two includes not strictly needed, as included later on,
// but doppelt gemoppelt haelt besser ;-)
#include <string.h>
#include <math.h>

// Includes of subroutines for eye tracking:
#include "remove_corneal_reflection.h"
#include "ransac_ellipse.h"
#include "svd.h"

// Includes from OpenCV:
#include "cv.h"
#include "highgui.h"

// Includes from PTB:
//
// It is important that these are included *last*, so remapping of some functions,
// e.g., printf() -> mexFunction() works correctly!
#include <Psych.h>
#include <PsychCV.h>


// Includes from Psychtoolbox core:
void Start_Timer(void)
{
	// Nothing to do.
	return;
}

double Time_Elapsed(void)
{
	double now;
	PsychGetAdjustedPrecisionTimerSeconds(&now);
	return(now);
}

#define UINT8 unsigned char

#ifndef PI
#define PI 3.141592653589
#endif

#define DEBUG 1

#define INFO(args...) if (DEBUG) printf(args)

#define CLIP(x,l,u) ((x)<(l)?((l)):((x)>(u)?(u):(x)))
#define ISIN(x,l,u) ((x)<(l)?((0)):((x)>(u)?(0):(1)))

#define CALIBRATIONPOINTS    9

FILE *logfile;
#define Log(args...) if (logfile) fprintf(logfile,args);

FILE *ellipse_log;

#define MIN_PUPIL_CONTOUR_POINTS  	500   
#define MAX_PUPIL_CONTOUR_POINTS  	10000    
#define PUPIL_SIZE_TOLERANCE 		1000	//range of allowed pupil diameters
#define MAX_CONTOUR_COUNT		20

// Firewire Capture Variables
//int dev;
int width,height,framerate=30;
int swidth, sheight;
FILE* imagefile;

//dc1394_cameracapture cameras[2];
//int numNodes;
//int numCameras;
//raw1394handle_t handle;
//nodeid_t * camera_nodes;
//dc1394_feature_set features;

// Load the source image. 
IplImage *eye_image=NULL;
IplImage *original_eye_image=NULL;
IplImage *cornea_eye_image=NULL;
IplImage *threshold_image=NULL;
IplImage *ellipse_image=NULL;
IplImage *scene_image=NULL;

// Window handles
const char* eye_window = "Eye Image Window";
const char* original_eye_window = "Original Eye Image";
const char* ellipse_window = "Fitted Ellipse Window";
const char* scene_window = "Scene Image Window";
const char* control_window = "Parameter Control Window";

/*
char Feature_Names[9][30] ={
 "BRIGHTNESS",
 "EXPOSURE",
 "SHARPNESS",
 "WHITE BALANCE",
 "HUE",
 "SATURATION",
 "GAMMA",
 "SHUTTER",
 "GAIN"};

typedef struct {
    int offset_value;
    int value;
    int min;
    int max;
    int available;   
    void (*callback)(int);
} camera_features;

camera_features eye_camera_features[9];
*/

CvPoint pupil = {0,0};					// coordinates of pupil in tracker coordinate system
CvPoint corneal_reflection = {0,0};		// coordinates of corneal reflection in tracker coordinate system
CvPoint diff_vector = {0,0};			// vector between the corneal reflection and pupil
int corneal_reflection_r = 0;			// the radius of corneal reflection
CvPoint gaze_point = {0,0};				// the mapped/recovered point of gaze
int lost_frame_num = 0;					// "valid" flag: Small numbers mean - Tracking was successfull.
unsigned int trackedframes = 0;			// Serial count of tracked eye-frames.

int view_cal_points = 1;
int do_map2scene = 0;					// Enable gaze point calculation based on current 9-point calibration.

int number_calibration_points_set = 0;
int ok_calibrate = 0;

CvPoint  calipoints[CALIBRATIONPOINTS];       //conversion from eye to scene calibration points
CvPoint  scenecalipoints[CALIBRATIONPOINTS]; //captured (with mouse) calibration points
CvPoint  pucalipoints[CALIBRATIONPOINTS];   //captured eye points while looking at the calibration points in the scene
CvPoint  crcalipoints[CALIBRATIONPOINTS];    //captured corneal reflection points while looking at the calibration points in the scene
CvPoint  vectors[CALIBRATIONPOINTS];         //differences between the corneal reflection and pupil center

//scene coordinate interpolation variables
float a, b, c, d, e;                            //temporary storage of coefficients
float aa, bb, cc, dd, ee;                       //pupil X coefficients    
float ff, gg, hh, ii, jj;			//pupil Y coefficients 

float centx, centy;                             // translation to center pupil data after biquadratics
float cmx[4], cmy[4];                           // corner correctioncoefficients 
int inx, iny;                                   // translation to center pupil data before biquadratics

CvScalar White, Red, Green, Blue, Yellow;
int frame_number=0;

#define FRAMEW width
#define FRAMEH height

int monobytesperimage=FRAMEW*FRAMEH;
int yuv411bytesperimage=FRAMEW*FRAMEH*12/8;

//int cameramode[2]={MODE_640x480_MONO,MODE_640x480_YUV411};

const double beta = 0.2;			// hysteresis factor for noise reduction
double *intensity_factor_hori;		// horizontal intensity factor for noise reduction
double *avg_intensity_hori;			// horizontal average intensity

// Parameters for the algorithm: The defaults set here are meaningless, as this gets
// initialized at tracker init time by a high-level call from PsychCV:
int edge_threshold = 20;			// threshold of pupil edge points detection
int rays = 18;						// number of rays to use to detect feature points
int min_feature_candidates = 10;	// minimum number of pupil feature candidates
int cr_window_size = 301;			// corneal refelction search window size
int gausswidth = 5;					// MK: Size of gaussian low pass blur window.
double maxeccentricity = 2.0;		// MK: Max eccentricity of pupil best fit ellipse.
double initial_angle_spread = 2*PI;	// MK: Spread angle of rays in initial starburst scan.
double fanoutangle1 = 0.0;			// MK: If initial_angle_spread < 2PI, ie. we use two fans of beams,
double fanoutangle2 = PI;			// MK: Then fanoutangle1 and fanoutangle2 encode the start (middle) angles of both fans.
int bouncerays = 0x0;				// MK: If set to & 0x1, then Starburst will use bounced/reflected rays to add more features, otherwise not.
int features_per_ray = 2;			// MK: Try to add up to features_per_ray features per Starburst beam.

double	min_feature_dist = 0.0;			// MK: Minimum distance of a starburst feature from start point to be accepted.
double 	max_feature_dist = 1000.0;		// MK: Maximum distance of a starburst feature from start point to be accepted.
double 	min_ellipse_area = 0.0;			// MK: Not area - Currently min length of larger ellipse half-axis for acceptance by RANSAC.
double 	max_ellipse_area = 1000000.0;	// MK: Not area - Currently max length of larger ellipse half-axis for acceptance by RANSAC.

bool detectCornealReflection = TRUE;	// MK: Auto-Detection of reference point (corneal reflection) enabled?
int nrInChannels;						// MK: Number of color input channels.
bool noise_reduction = FALSE;			// MK: Enable/Disable line noise reduction.

double map_matrix[3][3];
int save_image = 0;
int image_no = 0;
int save_ellipse = 0;
int ellipse_no = 0;
char eye_file[30];
char scene_file[30];
char ellipse_file[40];

#define YUV2RGB(y, u, v, r, g, b)\
  r = y + ((v*1436) >> 10);\
  g = y - ((u*352 + v*731) >> 10);\
  b = y + ((u*1814) >> 10);\
  r = r < 0 ? 0 : r;\
  g = g < 0 ? 0 : g;\
  b = b < 0 ? 0 : b;\
  r = r > 255 ? 255 : r;\
  g = g > 255 ? 255 : g;\
  b = b > 255 ? 255 : b

#define FIX_UINT8(x) ( (x)<0 ? 0 : ((x)>255 ? 255:(x)) )

/*
//----------------------- Firewire Image Capture Code -----------------------//

void Open_IEEE1394() 
{
  int i;

  handle = dc1394_create_handle(0);
  if (handle==NULL) {
    fprintf( stderr, "Unable to aquire a raw1394 handle\n\n"
	"Please check \n"
	"  - if the kernel modules `ieee1394',`raw1394' and `ohci1394' are loaded \n"
	"  - if you have read/write access to /dev/raw1394\n\n");
    exit(1);
  }

  numNodes = raw1394_get_nodecount(handle);
  camera_nodes = dc1394_get_camera_nodes(handle,&numCameras,1);
  fflush(stdout);
  if (numCameras<1) {
    fprintf( stderr, "no cameras found :(\n");
    dc1394_destroy_handle(handle);
    exit(1);
  }

  for (i = 0; i < numCameras; i++) {
    dc1394_camera_on(handle, camera_nodes[i]);

    if (dc1394_dma_setup_capture(handle,camera_nodes[i],
			i,  
			FORMAT_VGA_NONCOMPRESSED,
			cameramode[i],
			SPEED_400,
			FRAMERATE_30,40,1,"/dev/video1394",
			&cameras[i])!=DC1394_SUCCESS) {
      fprintf( stderr,"unable to setup camera\n");
      dc1394_release_camera(handle,&cameras[i]);
      dc1394_destroy_handle(handle);
      exit(1);
    }
    if (dc1394_start_iso_transmission(handle,cameras[i].node) !=DC1394_SUCCESS) {
      fprintf( stderr, "unable to start camera iso transmission\n");
      dc1394_release_camera(handle,&cameras[i]);
      dc1394_destroy_handle(handle);
      exit(1);
    }
    printf("Camera %d Open\n",i);
  }
}

void Grab_IEEE1394() 
{
  if (dc1394_dma_multi_capture(cameras, numCameras)!=DC1394_SUCCESS) {
    fprintf( stderr, "unable to capture a frame\n");
  }
}

void Release_IEEE1394() 
{
  int i;

  for (i=0; i<numCameras; i++) {
    dc1394_dma_done_with_buffer(&cameras[i]);
  }
}

void Close_IEEE1394() 
{
  int i;

  for (i=0; i<numCameras; i++) {
    if (dc1394_stop_iso_transmission(handle,cameras[i].node)!=DC1394_SUCCESS) {
      printf("couldn't stop the camera?\n");
    }
    dc1394_camera_off(handle, cameras[i].node); 
    dc1394_dma_release_camera(handle,&cameras[i]);
  }
  dc1394_destroy_handle(handle);
}

*/

//------------ map pupil coordinates to screen coordinates ---------/
CvPoint homography_map_point(CvPoint p)
{
  CvPoint p2;
  double z = map_matrix[2][0]*p.x + map_matrix[2][1]*p.y + map_matrix[2][2];
  p2.x = (int)((map_matrix[0][0]*p.x + map_matrix[0][1]*p.y + map_matrix[0][2])/z);
  p2.y = (int)((map_matrix[1][0]*p.x + map_matrix[1][1]*p.y + map_matrix[1][2])/z);
  return p2;
}

// r is result matrix
void affine_matrix_inverse(double a[][3], double r[][3])
{
  double det22 = a[0][0]*a[1][1] - a[0][1]*a[1][0];
  r[0][0] = a[1][1]/det22;
  r[0][1] = -a[0][1]/det22;
  r[1][0] = -a[1][0]/det22;
  r[1][1] = a[0][0]/det22;

  r[2][0] = r[2][1] = 0;
  r[2][2] = 1/a[2][2];

  r[0][2] = -r[2][2] * (r[0][0]*a[0][2] + r[0][1]*a[1][2]);
  r[1][2] = -r[2][2] * (r[1][0]*a[0][2] + r[1][1]*a[1][2]);
}

// r is result matrix
void matrix_multiply33(double a[][3], double b[][3], double r[][3])
{
  int i, j;
  double result[9];
  double v = 0;
  for (j = 0; j < 3; j++) {
    for (i = 0; i < 3; i++) {
      v = a[j][0]*b[0][i];
      v += a[j][1]*b[1][i];
      v += a[j][2]*b[2][i];
      result[j*3+i] = v;
    }
  }
  for (i = 0; i < 3; i++) {
    r[i][0] = result[i*3];
    r[i][1] = result[i*3+1];
    r[i][2] = result[i*3+2];
  }
}

int cal_calibration_homography(void)
{
  int i, j;
  stuDPoint cal_scene[9], cal_eye[9];
  stuDPoint scene_center, eye_center, *eye_nor, *scene_nor;
  double dis_scale_scene, dis_scale_eye;  

  for (i = 0; i < 9; i++) {
    cal_scene[i].x = scenecalipoints[i].x;  
    cal_scene[i].y = scenecalipoints[i].y;
    cal_eye[i].x = vectors[i].x;
    cal_eye[i].y = vectors[i].y;
  }

  scene_nor = normalize_point_set(cal_scene, dis_scale_scene, scene_center, CALIBRATIONPOINTS);
  eye_nor = normalize_point_set(cal_eye, dis_scale_eye, eye_center, CALIBRATIONPOINTS);

  printf("normalize_point_set end\n");
  printf("scene scale:%lf  center (%lf, %lf)\n", dis_scale_scene, scene_center.x, scene_center.y);
  printf("eye scale:%lf  center (%lf, %lf)\n", dis_scale_eye, eye_center.x, eye_center.y);

  const int homo_row=18, homo_col=9;
  double A[homo_row][homo_col];
  int M = homo_row, N = homo_col; //M is row; N is column
  double **ppa = (double**)malloc(sizeof(double*)*M);
  double **ppu = (double**)malloc(sizeof(double*)*M);
  double **ppv = (double**)malloc(sizeof(double*)*N);
  double pd[homo_col];
  for (i = 0; i < M; i++) {
    ppa[i] = A[i];
    ppu[i] = (double*)malloc(sizeof(double)*N);
  }
  for (i = 0; i < N; i++) {
    ppv[i] = (double*)malloc(sizeof(double)*N);
  }

  for (j = 0;  j< M; j++) {
    if (j%2 == 0) {
      A[j][0] = A[j][1] = A[j][2] = 0;
      A[j][3] = -eye_nor[j/2].x;
      A[j][4] = -eye_nor[j/2].y;
      A[j][5] = -1;
      A[j][6] = scene_nor[j/2].y * eye_nor[j/2].x;
      A[j][7] = scene_nor[j/2].y * eye_nor[j/2].y;
      A[j][8] = scene_nor[j/2].y;
    } else {
      A[j][0] = eye_nor[j/2].x;
      A[j][1] = eye_nor[j/2].y;
      A[j][2] = 1;
      A[j][3] = A[j][4] = A[j][5] = 0;
      A[j][6] = -scene_nor[j/2].x * eye_nor[j/2].x;
      A[j][7] = -scene_nor[j/2].x * eye_nor[j/2].y;
      A[j][8] = -scene_nor[j/2].x;
    }
  }

  printf("normalize_point_set end\n");

  svd(M, N, ppa, ppu, pd, ppv);
  int min_d_index = 0;
  for (i = 1; i < N; i++) {
    if (pd[i] < pd[min_d_index])
      min_d_index = i;
  }

  for (i = 0; i < N; i++) {
      map_matrix[i/3][i%3] = ppv[i][min_d_index];  //the column of v that corresponds to the smallest singular value,
                                                //which is the solution of the equations
  }

  double T[3][3] = {0}, T1[3][3] = {0};
  printf("\nT1: \n");
  for (j = 0; j < 3; j++) {
    for (i = 0; i < 3; i++) {
      printf("%8lf ", T1[j][i]);
    }
    printf("\n");
  }  

  T[0][0] = T[1][1] = dis_scale_eye;
  T[0][2] = -dis_scale_eye*eye_center.x;
  T[1][2] = -dis_scale_eye*eye_center.y;
  T[2][2] = 1;

  printf("\nmap_matrix: \n");
  for (j = 0; j < 3; j++) {
    for (i = 0; i < 3; i++) {
      printf("%8lf ", map_matrix[j][i]);
    }
    printf("\n");
  }   
  printf("\nT: \n");
  for (j = 0; j < 3; j++) {
    for (i = 0; i < 3; i++) {
      printf("%8lf ", T[j][i]);
    }
    printf("\n");
  }  

  matrix_multiply33(map_matrix, T, map_matrix); 

  T[0][0] = T[1][1] = dis_scale_scene;
  T[0][2] = -dis_scale_scene*scene_center.x;
  T[1][2] = -dis_scale_scene*scene_center.y;
  T[2][2] = 1;

  printf("\nmap_matrix: \n");
  for (j = 0; j < 3; j++) {
    for (i = 0; i < 3; i++) {
      printf("%8lf ", map_matrix[j][i]);
    }
    printf("\n");
  } 
  printf("\nT: \n");
  for (j = 0; j < 3; j++) {
    for (i = 0; i < 3; i++) {
      printf("%8lf ", T[j][i]);
    }
    printf("\n");
  }   

  affine_matrix_inverse(T, T1);
  matrix_multiply33(T1, map_matrix, map_matrix);

  printf("\nmap_matrix: \n");
  for (j = 0; j < 3; j++) {
    for (i = 0; i < 3; i++) {
      printf("%8lf ", map_matrix[j][i]);
    }
    printf("\n");
  }   

  for (i = 0; i < M; i++) {
    free(ppu[i]);
  }
  for (i = 0; i < N; i++) {
    free(ppv[i]);
  }
  free(ppu);
  free(ppv);
  free(ppa);
      
  free(eye_nor);
  free(scene_nor);
  printf("\nfinish calculate calibration\n");
}

CvPoint map_point(CvPoint p)      
{
 CvPoint p2;
 int quad=0;
 float x1,y1,xx,yy;

 // correct eye position by recentering offset:
 x1 = (float) p.x;    
 y1 = (float) p.y;

 // translate before biquadratic:
 x1 -= inx;        
 y1 -= iny;

 // biquadratic mapping:
 xx = aa+bb*x1+cc*y1+dd*x1*x1+ee*y1*y1;   
 yy = ff+gg*x1+hh*y1+ii*x1*x1+jj*y1*y1;

 // translate after biquadratic:
 x1 = xx - centx;                 
 y1 = yy - centy;

 // determine quadrant of point:
 if      (( x1<0 )&&( y1<0 )) quad = 0;   
 else if (( x1>0 )&&( y1<0 )) quad = 1;
 else if (( x1<0 )&&( y1>0 )) quad = 2;
 else if (( x1>0 )&&( y1>0 )) quad = 3;

 // fix up by quadrant:
 p2.x = (int)(xx + x1*y1*cmx[quad]);       
 p2.y = (int)(yy + x1*y1*cmy[quad]);

 return p2;
}



//---------- calibration  coefficient calculation ---------------//
// biquadratic equation fitter               
// x, y are coordinates of eye tracker point 
// X is x or y coordinate of screen point    
// computes a, b, c, d, and e in the biquadratic 
// X = a + b*(x-inx) + c*(y-iny) + d*(x-inx)*(x-inx) + e*(y-iny)*(y-iny) 
// where inx = x1, y1 = y1 to reduce the solution to a 4x4 matrix        

void dqfit( float x1, float y1, 
	    float x2, float y2, 
	    float x3, float y3, 
	    float x4, float y4, 
	    float x5, float y5,
	    float X1, float X2, float X3, float X4, float X5 )
{
 float den;
 float x22,x32,x42,x52;    // squared terms 
 float y22,y32,y42,y52;

 inx = (int)x1;            // record eye tracker centering constants 
 iny = (int)y1;
 a = X1;                    // first coefficient 
 X2 -= X1;  X3 -= X1;       // center screen points 
 X4 -= X1;  X5 -= X1;
 x2 -= x1;  x3 -= x1;       // center eye tracker points 
 x4 -= x1;  x5 -= x1;
 y2 -= y1;  y3 -= y1;  
 y4 -= y1;  y5 -= y1;
 x22 = x2*x2; x32 = x3*x3;   // squared terms of biquadratic 
 x42 = x4*x4; x52 = x5*x5;
 y22 = y2*y2; y32 = y3*y3;
 y42 = y4*y4; y52 = y5*y5;

 //Cramer's rule solution of 4x4 matrix */
 den = -x2*y3*x52*y42-x22*y3*x4*y52+x22*y5*x4*y32-y22*x42*y3*x5-
    x32*y22*x4*y5-x42*x2*y5*y32+x32*x2*y5*y42-y2*x52*x4*y32+
    x52*x2*y4*y32+y22*x52*y3*x4+y2*x42*x5*y32+x22*y3*x5*y42-
    x32*x2*y4*y52-x3*y22*x52*y4+x32*y22*x5*y4-x32*y2*x5*y42+
    x3*y22*x42*y5+x3*y2*x52*y42+x32*y2*x4*y52+x42*x2*y3*y52-
    x3*y2*x42*y52+x3*x22*y4*y52-x22*y4*x5*y32-x3*x22*y5*y42;

 b =  (-y32*y2*x52*X4-X2*y3*x52*y42-x22*y3*X4*y52+x22*y3*y42*X5+
    y32*y2*x42*X5-y22*x42*y3*X5+y22*y3*x52*X4+X2*x42*y3*y52+
    X3*y2*x52*y42-X3*y2*x42*y52-X2*x42*y5*y32+x32*y42*y5*X2+
    X2*x52*y4*y32-x32*y4*X2*y52-x32*y2*y42*X5+x32*y2*X4*y52+
    X4*x22*y5*y32-y42*x22*y5*X3-x22*y4*y32*X5+x22*y4*X3*y52+
    y22*x42*y5*X3+x32*y22*y4*X5-y22*x52*y4*X3-x32*y22*y5*X4)/den;

 c =  (-x32*x4*y22*X5+x32*x5*y22*X4-x32*y42*x5*X2+x32*X2*x4*y52+
    x32*x2*y42*X5-x32*x2*X4*y52-x3*y22*x52*X4+x3*y22*x42*X5+
    x3*x22*X4*y52-x3*X2*x42*y52+x3*X2*x52*y42-x3*x22*y42*X5-
    y22*x42*x5*X3+y22*x52*x4*X3+x22*y42*x5*X3-x22*x4*X3*y52-
    x2*y32*x42*X5+X2*x42*x5*y32+x2*X3*x42*y52+x2*y32*x52*X4+
    x22*x4*y32*X5-x22*X4*x5*y32-X2*x52*x4*y32-x2*X3*x52*y42)/den;

 d = -(-x4*y22*y3*X5+x4*y22*y5*X3-x4*y2*X3*y52+x4*y2*y32*X5-
    x4*y32*y5*X2+x4*y3*X2*y52-x3*y22*y5*X4+x3*y22*y4*X5+
    x3*y2*X4*y52-x3*y2*y42*X5+x3*y42*y5*X2-x3*y4*X2*y52-
    y22*y4*x5*X3+y22*X4*y3*x5-y2*X4*x5*y32+y2*y42*x5*X3+
    x2*y3*y42*X5-y42*y3*x5*X2+X4*x2*y5*y32+y4*X2*x5*y32-
    y42*x2*y5*X3-x2*y4*y32*X5+x2*y4*X3*y52-x2*y3*X4*y52)/den;

 e = -(-x3*y2*x52*X4+x22*y3*x4*X5+x22*y4*x5*X3-x3*x42*y5*X2-
    x42*x2*y3*X5+x42*x2*y5*X3+x42*y3*x5*X2-y2*x42*x5*X3+
    x32*x2*y4*X5-x22*y3*x5*X4+x32*y2*x5*X4-x22*y5*x4*X3+
    x2*y3*x52*X4-x52*x2*y4*X3-x52*y3*x4*X2-x32*y2*x4*X5+
    x3*x22*y5*X4+x3*y2*x42*X5+y2*x52*x4*X3-x32*x5*y4*X2-
    x32*x2*y5*X4+x3*x52*y4*X2+x32*x4*y5*X2-x3*x22*y4*X5)/den;
}


int CalculateCalibration(void)
{
  int i, j;
  float x, y, wx[9], wy[9];   	//work data points
  int calx[10], caly[10];	//scene coordinate interpolation variables
  int eye_x[10], eye_y[10];	//scene coordinate interpolation variables

  // Place scene coordinates into calx and caly
  for(i = 0; i<9;i++) {
    calx[i] = scenecalipoints[i].x;  caly[i] = scenecalipoints[i].y;
  }

  // Set the last "tenth"  point
  calx[9] = scenecalipoints[0].x;  caly[9] = scenecalipoints[0].y;

  // Store pupil into eye_x and eye_y
  for(i = 0; i < 9; i++) {
   eye_x[i] = vectors[i].x;
   eye_y[i] = vectors[i].y;
  }

  // Solve X biquadratic
  dqfit((float)eye_x[0],(float)eye_y[0],(float)eye_x[1],(float)eye_y[1],(float)eye_x[2],   
        (float)eye_y[2],(float)eye_x[3],(float)eye_y[3],(float)eye_x[4],(float)eye_y[4],
        (float)calx[0],(float)calx[1],(float)calx[2],(float)calx[3],(float)calx[4]);
  aa = a; bb = b; cc = c; dd = d; ee = e;

  // Solve Y biquadratic
  dqfit((float)eye_x[0],(float)eye_y[0],(float)eye_x[1],(float)eye_y[1],(float)eye_x[2],
        (float)eye_y[2],(float)eye_x[3],(float)eye_y[3],(float)eye_x[4],(float)eye_y[4],
        (float)caly[0],(float)caly[1],(float)caly[2],(float)caly[3],(float)caly[4]);
  ff = a; gg = b; hh = c; ii = d; jj = e;

  // Biquadratic mapping of points
  for(i = 0; i < 9; i++) {
    x = (float)(eye_x[i] - inx);
    y = (float)(eye_y[i] - iny);
    wx[i] = aa+bb*x+cc*y+dd*x*x+ee*y*y;
    wy[i] = ff+gg*x+hh*y+ii*x*x+jj*y*y;
  }
  
  // Shift screen points to center for quadrant compute
  centx = wx[0];      
  centy = wy[0];
    
  // Normalize to center:
  for(i = 0; i < 9; i++) {
   wx[i] -= centx;
   wy[i] -= centy;
  }
  
  // Compute coefficents for each quadrant
  for(i = 0; i < 4; i++) {
   j = i + 5;
   cmx[i] = (calx[j]-wx[j]-centx)/(wx[j]*wy[j]);
   cmy[i] = (caly[j]-wy[j]-centy)/(wx[j]*wy[j]);
  }

  return 0;
}


void Draw_Cross(IplImage *image, int centerx, int centery, int x_cross_length, int y_cross_length, CvScalar color)
{
  CvPoint pt1,pt2,pt3,pt4;

  pt1.x = centerx - x_cross_length;
  pt1.y = centery;
  pt2.x = centerx + x_cross_length;
  pt2.y = centery;

  pt3.x = centerx;
  pt3.y = centery - y_cross_length;
  pt4.x = centerx;
  pt4.y = centery + y_cross_length;

  cvLine(image,pt1,pt2,color,1,8);
  cvLine(image,pt3,pt4,color,1,8);
}

void Show_Calibration_Points()
{
  int i; 
  for (i=0;i<CALIBRATIONPOINTS;i++) 
    Draw_Cross(scene_image, scenecalipoints[i].x, scenecalipoints[i].y, 25, 25, CV_RGB(255,255,255));
}

void Zero_Calibration() 
{ 
  int i;
 
  for (i=0;i<CALIBRATIONPOINTS;i++) {
    scenecalipoints[i].x = 0;  
    scenecalipoints[i].y = 0;
 
    pucalipoints[i].x = 0;    
    pucalipoints[i].y = 0;
    
    crcalipoints[i].x = 0;    
    crcalipoints[i].y = 0;

    vectors[i].x = 0; 
    vectors[i].y = 0;
  }
  number_calibration_points_set=0;
  do_map2scene = 0;
}

void Set_Calibration_Point(int x, int y)
{

   if (number_calibration_points_set<CALIBRATIONPOINTS) {

     //store xy mouse "scene" coordinates into calibration array    
     scenecalipoints[number_calibration_points_set].x = x;
     scenecalipoints[number_calibration_points_set].y = y;

     //grab the "pupil" position
     pucalipoints[number_calibration_points_set].x = pupil.x;
     pucalipoints[number_calibration_points_set].y = pupil.y;
       
     //grab the "corneal reflection" points  
     crcalipoints[number_calibration_points_set].x = corneal_reflection.x;   
     crcalipoints[number_calibration_points_set].y = corneal_reflection.y;   
       
     //grab the "delta pupil cr" position 
     vectors[number_calibration_points_set].x = diff_vector.x; 
     vectors[number_calibration_points_set].y = diff_vector.y; 

     number_calibration_points_set++;
     printf("calibration points number: %d (total 9)\n", number_calibration_points_set);
   } else {

     Zero_Calibration();

   }
}

void Set_Calibration_Point1(int x, int y)
{

   if (number_calibration_points_set<CALIBRATIONPOINTS) {

     //store xy mouse "scene" coordinates into calibration array    
     scenecalipoints[number_calibration_points_set].x = x;
     scenecalipoints[number_calibration_points_set].y = y;

     //grab the "pupil" position
     pucalipoints[number_calibration_points_set].x = pupil.x;
     pucalipoints[number_calibration_points_set].y = pupil.y;
       
     //grab the "corneal reflection" points  
     //crcalipoints[number_calibration_points_set].x = corneal_reflection.x;   
     //crcalipoints[number_calibration_points_set].y = corneal_reflection.y;   
       
     //grab the "delta pupil cr" position 
     vectors[number_calibration_points_set].x = pupil.x; 
     vectors[number_calibration_points_set].y = pupil.y; 

     number_calibration_points_set++;

   } else {

     Zero_Calibration();

   }
}

void Activate_Calibration() 
{
  int i;
  int calibration_result;

  INFO("Map eye to scene image\n");

  if (number_calibration_points_set==CALIBRATIONPOINTS) {
    //calibration_result = CalculateCalibration();
    calibration_result = cal_calibration_homography();
 
    INFO("Calibration result = %d\n", calibration_result);
 
    do_map2scene = 1;

    //do_map2scene = !do_map2scene;
    view_cal_points = !view_cal_points;

    INFO("Scene coordinates:\n");
    for(i=0;i< CALIBRATIONPOINTS;i++) {
      INFO("pt %d x = %d , y = %d \n", i, scenecalipoints[i].x, scenecalipoints[i].y);
    }
    INFO("\n");

    INFO("Eye coordinates\n");
    for(i=0;i< CALIBRATIONPOINTS;i++) {
      INFO("pt %d x = %d , y = %d \n", i, pucalipoints[i].x, pucalipoints[i].y);
    }
    INFO("\n");

    INFO("Corneal reflection coordinates\n");
    for(i=0;i< CALIBRATIONPOINTS;i++) {
      INFO("pt %d x = %d , y = %d \n", i, crcalipoints[i].x, crcalipoints[i].y);
    }
    INFO("\n");
  } else {
    INFO("Attempt to activate calibration without a full set of points.\n");
  }
   
}


void on_mouse_scene( int event, int x, int y, int flags, void* param)
{
   int i;

   switch (event) {
     //This is really the left mouse button
     case CV_EVENT_LBUTTONDOWN:
       Set_Calibration_Point(x,y);
       break;
    
     //This is really the right mouse button
     case CV_EVENT_MBUTTONDOWN:
       Activate_Calibration();
       break;
     
     //This is really the scroll button
     case CV_EVENT_RBUTTONDOWN:
       break;
   }
}

void on_mouse_eye( int event, int x, int y, int flags, void* param)
{
   int i;
   static bool start = 0;

   switch (event) {
     //This is really the left mouse button
     case CV_EVENT_LBUTTONDOWN:
       printf("left mouse eye window (%d,%d)\n", x, y);
       pupil.x = x;
       pupil.y = y;
       //if (!start) { 
         printf("start point: %d, %d\n", x, y); 
         start_point.x = x;
         start_point.y = y;
         start = 1;
       //}
       break;
    
     //This is really the right mouse button
     case CV_EVENT_MBUTTONDOWN:
        break;
     
     //This is really the scroll button
     case CV_EVENT_RBUTTONDOWN:
       break;
   }
}


void Average_Frames(UINT8 *result_image, UINT8 *prev_image, UINT8 *now_image, UINT8 *next_image)
{
  int npixels = FRAMEW * FRAMEH;
  int i;
  for (i = 0; i < npixels; i++) {
    *result_image = (*prev_image + *now_image + *next_image) / 3;
    result_image++;
    prev_image++;
    now_image++;
    next_image++;
  }
}

void Normalize_Line_Histogram(IplImage *in_image) 
{
 unsigned char *s=(unsigned char *)in_image->imageData;
 int x,y;
 int linesum;
 double factor=0;
 int subsample=10;
 double hwidth=(100.0f*(double)width/(double)subsample);
/*
 char adjustment;
 for (y=0;y<height;y++) {
   linesum=0; 
   for (x=0;x<width;x+=subsample) {
     linesum+=*s;
     s+=subsample;
   }
   s-=width;
   adjustment=(char)(128-(double)(linesum)/(double)(width/subsample));
   for (x=0;x<width;x++) {
     *s=MIN(*s+adjustment,255);
     s++;
   }
 }
*/
 for (y=0;y<height;y++) {
   linesum=1; 
   for (x=0;x<width;x+=subsample) {
     linesum+=*s;
     s+=subsample;
   }
   s-=width;
   factor=hwidth/((double)linesum);
   for (x=0;x<width;x++) {
     *s=(unsigned char)(((double)*s)*factor);
     s++;
   }
 }
}


void Calculate_Avg_Intensity_Hori(IplImage* in_image)
{
  UINT8 *pixel = (UINT8*)in_image->imageData;
  int sum;
  int i, j;
  for (j = 0; j < in_image->height; j++) {
    sum = 0;
    for (i = 0; i < in_image->width; i++) {
      sum += *pixel;
      pixel++;
    }
    avg_intensity_hori[j] = (double)sum/in_image->width;
  }
}

void Reduce_Line_Noise(IplImage* in_image)
{
  if (!noise_reduction) return;
  
  UINT8 *pixel = (UINT8*)in_image->imageData;
  int i, j;
  double beta2 = 1 - beta;
  int adjustment;

  Calculate_Avg_Intensity_Hori(in_image);
  for (j = 0; j < in_image->height; j++) {
    intensity_factor_hori[j] = avg_intensity_hori[j]*beta + intensity_factor_hori[j]*beta2;
    adjustment = (int)(intensity_factor_hori[j] - avg_intensity_hori[j]);
    for (i = 0; i < in_image->width; i++) {
      *pixel =  FIX_UINT8(*pixel+adjustment);
      pixel++;
    }
  }
}

//----------------------- uyyvyy (i.e. YUV411) to rgb24 -----------------------//
void uyyvyy2rgb (unsigned char *src, unsigned char *dest, unsigned long long int NumPixels)
{
  register int i = NumPixels + ( NumPixels >> 1 )-1;
  register int j = NumPixels + ( NumPixels << 1 )-1;
  register int y0, y1, y2, y3, u, v;
  register int r, g, b;

  while (i > 0) {
    y3 = (unsigned char) src[i--];
    y2 = (unsigned char) src[i--];
    v  = (unsigned char) src[i--] - 128;
    y1 = (unsigned char) src[i--];
    y0 = (unsigned char) src[i--];
    u  = (unsigned char) src[i--] - 128;
    YUV2RGB (y3, u, v, r, g, b);
    dest[j--] = r;
    dest[j--] = g;
    dest[j--] = b;
    YUV2RGB (y2, u, v, r, g, b);
    dest[j--] = r;
    dest[j--] = g;
    dest[j--] = b;
    YUV2RGB (y1, u, v, r, g, b);
    dest[j--] = r;
    dest[j--] = g;
    dest[j--] = b;
    YUV2RGB (y0, u, v, r, g, b);
    dest[j--] = r;
    dest[j--] = g;
    dest[j--] = b;
  }
}

void FirewireFrame_to_RGBIplImage(void *FirewireFrame, IplImage *OpenCV_image)
{
  uyyvyy2rgb((unsigned char *)FirewireFrame, (unsigned char *)OpenCV_image->imageData, swidth * sheight);
}

void Grab_Camera_Frames()
{
//  Grab_IEEE1394();

//  memcpy(eye_image->imageData,(char *)cameras[0].capture_buffer,monobytesperimage);
  //memcpy(scene_image->imageData,(char *)cameras[1].capture_buffer,monobytesperimage);
//  FirewireFrame_to_RGBIplImage((unsigned char *)cameras[1].capture_buffer, scene_image);

//  original_eye_image = cvCloneImage(eye_image);

  if (nrInChannels > 1) {
	// Input image is color RGB or RGBA. Convert to suitable color-space, classify and
	// store result into eye_image:
	
	if (nrInChannels == 3) {
		// Convert to HLS color space:
		if (gausswidth > 0) cvSmooth(original_eye_image, original_eye_image, CV_GAUSSIAN, gausswidth, gausswidth);

		cvCvtColor(original_eye_image, cornea_eye_image, CV_RGB2GRAY);
		cvCvtColor(original_eye_image, original_eye_image, CV_RGB2HLS);
//HUE Useless:		cvSplit(original_eye_image, eye_image, NULL, NULL, NULL);
		cvSplit(original_eye_image, NULL, eye_image, NULL, NULL);
//		cvSplit(original_eye_image, NULL, NULL, eye_image, NULL);
//		cvSplit(original_eye_image, NULL, NULL, NULL, eye_image);
	}
	
	if (nrInChannels == 4) {
		// Convert to grayscale image:
		cvCvtColor(original_eye_image, eye_image, CV_RGBA2GRAY);
	}
  }

  if (frame_number == 0) {
    Calculate_Avg_Intensity_Hori(eye_image);
    memcpy(intensity_factor_hori, avg_intensity_hori, eye_image->height*sizeof(double));    
  }    

  if (bouncerays & 0x2 && pupil_edge_thres > 1) cvThreshold(eye_image, eye_image, pupil_edge_thres, 255, CV_THRESH_BINARY);

//  Release_IEEE1394();
  frame_number++;
}

void process_image()
{
  int i, j;
  int *inliers_index;
  CvSize ellipse_axis;
  Grab_Camera_Frames();
  cvZero(ellipse_image);

  if (gausswidth > 0) cvSmooth(eye_image, eye_image, CV_GAUSSIAN, gausswidth, gausswidth);
  Reduce_Line_Noise(eye_image);  
  
  if (save_image == 1) {
    printf("save image %d\n", image_no);
    sprintf(eye_file, "./Eye/Eye_%05d.jpg", image_no);
    image_no++;
    cvSaveImage(eye_file, eye_image);
  }
  
  if (detectCornealReflection) {
	  //corneal reflection
	  remove_corneal_reflection((nrInChannels != 3) ? eye_image : cornea_eye_image, threshold_image, (int)start_point.x, (int)start_point.y, cr_window_size, 
								(int)eye_image->height/10, corneal_reflection.x, corneal_reflection.y, corneal_reflection_r);  
	  printf("corneal reflection: (%d, %d)\n", corneal_reflection.x, corneal_reflection.y);
  }

  Draw_Cross(ellipse_image, corneal_reflection.x, corneal_reflection.y, 15, 15, Yellow);  

  //starburst pupil contour detection
  starburst_pupil_contour_detection((UINT8*)eye_image->imageData, eye_image->width, eye_image->height,
                                edge_threshold, rays, min_feature_candidates, initial_angle_spread,
								fanoutangle1, fanoutangle2, bouncerays, features_per_ray, min_feature_dist, max_feature_dist);
  
  inliers_num = 0;
  inliers_index = pupil_fitting_inliers((UINT8*)eye_image->imageData, eye_image->width, eye_image->height, inliers_num, maxeccentricity, min_ellipse_area, max_ellipse_area);
  ellipse_axis.width = (int)pupil_param[0];
  ellipse_axis.height = (int)pupil_param[1];
  pupil.x = (int)pupil_param[2];
  pupil.y = (int)pupil_param[3];
  Draw_Cross(ellipse_image, pupil.x, pupil.y, 15, 15, Red);
  cvLine(eye_image, pupil, corneal_reflection, Red, 4, 8);
  cvLine(ellipse_image, pupil, corneal_reflection, Red, 4, 8);
  
  printf("ellipse a:%lf; b:%lf, cx:%lf, cy:%lf, theta:%lf; inliers_num:%d\n\n", 
         pupil_param[0], pupil_param[1], pupil_param[2], pupil_param[3], pupil_param[4], inliers_num);

  bool is_inliers = 0;
  for (int i = 0; i < edge_point.size(); i++) {
    is_inliers = 0;
    for (int j = 0; j < inliers_num; j++) {
      if (i == inliers_index[j])
        is_inliers = 1;
    }
    stuDPoint *edge = edge_point.at(i);
    if (is_inliers)
      Draw_Cross(ellipse_image, (int)edge->x,(int)edge->y, 5, 5, Green);
    else
      Draw_Cross(ellipse_image, (int)edge->x,(int)edge->y, 3, 3, Yellow);
  }
  free(inliers_index);

  if (ellipse_axis.width > 0 && ellipse_axis.height > 0) {
    start_point.x = pupil.x;
    start_point.y = pupil.y;
    //printf("start_point: %d,%d\n", start_point.x, start_point.y);
    Draw_Cross(eye_image, pupil.x, pupil.y, 10, 10, Green);
    cvEllipse(eye_image, pupil, ellipse_axis, -pupil_param[4]*180/PI, 0, 360, Red, 2);
    cvEllipse(ellipse_image, pupil, ellipse_axis, -pupil_param[4]*180/PI, 0, 360, Green, 2);

    diff_vector.x = pupil.x - corneal_reflection.x;
    diff_vector.y = pupil.y - corneal_reflection.y;
    if (do_map2scene) {
      gaze_point = homography_map_point(diff_vector);
      printf("gaze_point: (%d,%d)\n", gaze_point.x, gaze_point.y);  
      Draw_Cross(scene_image, gaze_point.x, gaze_point.y, 60, 60, Red);
    }
    lost_frame_num = 0;    
  } else {
    lost_frame_num++;
  }
  if (lost_frame_num > 5) {
    start_point.x = FRAMEW/2;
    start_point.y = FRAMEH/2;
  }
  Draw_Cross(ellipse_image, (int)start_point.x, (int)start_point.y, 7, 7, Blue);
  Draw_Cross(eye_image, (int)start_point.x, (int)start_point.y, 7, 7, Blue);

  if (save_ellipse == 1) {
    printf("save ellipse %d\n", ellipse_no);
    sprintf(ellipse_file, "./Ellipse/Ellipse_%05d.jpg", ellipse_no);
    ellipse_no++;
    cvSaveImage(ellipse_file, ellipse_image);
    fprintf(ellipse_log, "%.3f\t %8.2lf %8.2lf %8.2lf %8.2lf %8.2lf\n",
            Time_Elapsed(), pupil_param[0], pupil_param[1], pupil_param[2], pupil_param[3], pupil_param[4]);
  }
  
  printf("Time elapsed: %.3f\n", Time_Elapsed()); 
  if (logfile) fprintf(logfile,"%.3f\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\n",
					Time_Elapsed(),
					pupil.x,    
					pupil.y,    
					corneal_reflection.x,    
					corneal_reflection.y,
					diff_vector.x,    
					diff_vector.y,    
					gaze_point.x,    
					gaze_point.y);    

  if (view_cal_points) Show_Calibration_Points();
}

void Update_Gui_Windows() 
{
  static int first = 1;

  cvShowImage(eye_window, eye_image);
  cvShowImage(original_eye_window, original_eye_image);
  cvReleaseImage(&original_eye_image);
  cvShowImage(scene_window, scene_image);
  cvShowImage(ellipse_window, ellipse_image);

  if (first) {
	  cvResizeWindow(eye_window,320,240);
	  cvResizeWindow(original_eye_window,320,240);
	  cvResizeWindow(scene_window,320,240);
	  cvResizeWindow(ellipse_window,320,240);
	  cvSetTrackbarPos("Edge Threshold", control_window, pupil_edge_thres);
	  first = 0;
  }
}

void Open_GUI()
{
  int i;

  //Make the eye image (in monochrome):
  eye_image=cvCreateImageHeader(cvSize(width, height), 8, 1 );
  eye_image->imageData=(char *) malloc(width * height);

  //Make the eye image (in monochrome):
  threshold_image = cvCloneImage(eye_image);

  cornea_eye_image = cvCloneImage(eye_image);
   
  //Make the eye image (in RGB) :
  original_eye_image=cvCreateImageHeader(cvSize(width, height), 8, nrInChannels);
  original_eye_image->imageData=(char *) malloc(width * height * nrInChannels);

  //Make the ellipse image (in RGB) :
  ellipse_image=cvCreateImageHeader(cvSize(width, height), 8, 3 );
  ellipse_image->imageData=(char *) malloc(width * height * 3);
  
  //Make the scene image:    
  scene_image=cvCreateImageHeader(cvSize(swidth, sheight), 8, 3 );
  scene_image->imageData=(char *) malloc(swidth * sheight * 3);
/*
  //Create the windows
  cvNamedWindow(control_window, 0);
  cvNamedWindow(ellipse_window, 0);
  cvNamedWindow(scene_window, 0);
  cvNamedWindow(eye_window, 0);
  cvNamedWindow(original_eye_window, 0);

  //setup the mouse call back funtion here for calibration    
  cvSetMouseCallback(scene_window, on_mouse_scene);        
  cvSetMouseCallback(eye_window, on_mouse_eye);        

  cvCreateTrackbar("Edge Threshold", control_window, &pupil_edge_thres, 255, NULL );
  cvCreateTrackbar("Rays Number", control_window, &rays, 180, NULL );
  cvCreateTrackbar("Min Feature Candidates", control_window, &min_feature_candidates, 30, NULL );
  cvCreateTrackbar("Corneal Window Size",control_window, &cr_window_size, FRAMEH, NULL );
*/
  //Init colors
  White = CV_RGB(255,255,255);
  Red = CV_RGB(255,0,0);
  Green = CV_RGB(0,255,0);
  Blue = CV_RGB(0,0,255);
  Yellow = CV_RGB(255,255,0);
}

void Close_GUI() 
{
/*
  cvDestroyWindow(eye_window);
  cvDestroyWindow(original_eye_window);
  cvDestroyWindow(ellipse_window);
  cvDestroyWindow(scene_window);
  cvDestroyWindow(control_window);
*/
  cvReleaseImageHeader(&eye_image );
  cvReleaseImageHeader(&threshold_image ); 
  cvReleaseImageHeader(&original_eye_image );  
  cvReleaseImageHeader(&ellipse_image );
  cvReleaseImageHeader(&scene_image );
  cvReleaseImageHeader(&cornea_eye_image);
  
  cvReleaseImage(&eye_image);
  cvReleaseImage(&threshold_image);
  cvReleaseImage(&original_eye_image);
  cvReleaseImage(&ellipse_image);
  cvReleaseImage(&scene_image);
  cvReleaseImage(&cornea_eye_image);
}

void Open_Logfile(int argc, char** argv) 
{
  char defaultlogfilename[]="logfile.txt";
  char *logfilename;
  
  if (argc>1) {
    logfilename=argv[1];
  } else {
    logfilename=defaultlogfilename;
  }

  logfile=fopen(logfilename,"w+");

  if (logfile!=NULL) {
    fprintf(logfile,"Timestamp (seconds)\t pupil X\t pupil Y\t Scene X\t Scene Y\n");
  } else {
    fprintf(stderr,"Error opening logfile %s.",logfilename);
    exit(-1);
  }
}

void Close_Logfile() 
{
  fclose(logfile);
}

void Open_Ellipse_Log()
{
  static char *ellipse_log_name = "./Ellipse/ellipse_log.txt";
  ellipse_log = fopen(ellipse_log_name,"w+");

  if (logfile!=NULL) {
    fprintf(logfile,"Timestamp (seconds)\t a\t pupil b\t centerx\t centery\t theta\n");
  } else {
//    fprintf(stderr,"Error opening logfile %s.", ellipse_log_name);
//    exit(-1);
  }
}

/*
int main( int argc, char** argv )
{
  char c;

  // Open_IEEE1394();

  Open_GUI();

  Open_Logfile(argc,argv);

  Start_Timer();

  int i, j;
  double T[3][3], T1[3][3];
  for (j = 0; j < 3; j++) {
    for (i = 0; i < 3; i++) {
      T[j][i] = j*3+i+1;
    }
  }
  T[2][0] = T[2][1] = 0;
  printf("\nT: \n");
  for (j = 0; j < 3; j++) {
    for (i = 0; i < 3; i++) {
      printf("%6.2lf ", T[j][i]);
    }
    printf("\n");
  }
  affine_matrix_inverse(T, T1);
  printf("\nT1: \n");
  for (j = 0; j < 3; j++) {
    for (i = 0; i < 3; i++) {
      printf("%6.2lf ", T1[j][i]);
    }
    printf("\n");
  }
  

  
  
  while ((c=cvWaitKey(50))!='q') {
    if (c == 's') {
      sprintf(eye_file, "eye%05d.bmp", image_no);
      sprintf(scene_file, "scene%05d.bmp", image_no);
      image_no++;
      cvSaveImage(eye_file, eye_image);
      cvSaveImage(scene_file, scene_image);
      printf("thres: %d\n", pupil_edge_thres);
    } else if (c == 'c') {
      save_image = 1 - save_image;
      printf("save_image = %d\n", save_image);
    } else if (c == 'e') {
      save_ellipse = 1 - save_ellipse;
      printf("save_ellipse = %d\n", save_ellipse);
      if (save_ellipse == 1) {
        Open_Ellipse_Log();
      } else {
        fclose(ellipse_log);
      }
    }
    if (start_point.x == -1 && start_point.y == -1)
      Grab_Camera_Frames();
    else 
      process_image(); 
    if (frame_number%1==0) Update_Gui_Windows(); 
  }

  Close_Logfile();

  Close_GUI();

  Close_IEEE1394();

  return 0;
}
*/

// These functions are exported to the higher-level PsychCV.c wrapper for access by
// Psychtoolbox:

// Initially setup tracker, reset everything to defaults:
psych_bool cvEyeTrackerInitialize(const char* logfilename, int eyewidth, int eyeheight, int eyechannels, void** eyeInputImageMono8, void** eyeInputImageColor, int scenewidth, int sceneheight, void** sceneInputImageRGB8, void** ellipseOutputImageRGB8, void** thresholdOutputImageMono8)
{
	// Disable logfile by default:
	logfile = NULL;
	
	// Reset total frame counter:
	frame_number = 0;
	
	// Setup image dimensions:
	width = eyewidth;
	height = eyeheight;
	swidth = scenewidth;
	sheight = sceneheight;
	nrInChannels = eyechannels;
	
	intensity_factor_hori	= (double*) malloc(height * sizeof(double)); //horizontal intensity factor for noise reduction
	avg_intensity_hori		= (double*) malloc(height * sizeof(double)); //horizontal average intensity

	// Open the HighGUI based GUI, setup initial image data structures:
	Open_GUI();

	// Open logfile if requested:
	if (logfilename) {
		// Try to open the logfile for writeout in append mode:
		logfile=fopen(logfilename,"w+");
		if (logfile!=NULL) {
			fprintf(logfile,"Timestamp (seconds)\t pupil X\t pupil Y\t Scene X\t Scene Y\n");
		} else {
			printf("PTB-ERROR: Error opening logfile %s for OpenEyes eyetracker. Aborting...\n",logfilename);
			Close_GUI();
			return(FALSE);
		}		
	}
	
	// Assign return pointers to the buffers to be filled with captured image data:
	*eyeInputImageMono8 = eye_image->imageData;
	*eyeInputImageColor = (nrInChannels > 1) ? original_eye_image->imageData : eye_image->imageData;
	*sceneInputImageRGB8 = scene_image->imageData;
	*ellipseOutputImageRGB8 = ellipse_image->imageData;
	*thresholdOutputImageMono8 = threshold_image->imageData;

/*	
	// Setup initial tracking/calibration/remapping matrices:
	int i, j;
	double T[3][3], T1[3][3];
	for (j = 0; j < 3; j++) {
		for (i = 0; i < 3; i++) {
			T[j][i] = j*3+i+1;
		}
	}
	T[2][0] = T[2][1] = 0;
//	printf("\nT: \n");
	for (j = 0; j < 3; j++) {
		for (i = 0; i < 3; i++) {
//			printf("%6.2lf ", T[j][i]);
		}
//		printf("\n");
	}
	affine_matrix_inverse(T, T1);
//	printf("\nT1: \n");
	for (j = 0; j < 3; j++) {
		for (i = 0; i < 3; i++) {
//			printf("%6.2lf ", T1[j][i]);
		}
//		printf("\n");
	}
*/
	
	// Reset start point for pupil location to "undefined":
	start_point.x = -1;
	start_point.y = -1;
	lost_frame_num = 1000;
	trackedframes = 0;

	// Done.
	return(TRUE);
}

// Shutdown and release tracker:
psych_bool cvEyeTrackerShutdown(void)
{
	// Close logfile if any open:
	if (logfile) { Close_Logfile(); logfile = NULL; }
	
	// Close GUI windows and release data structures:
	Close_GUI();

	free(intensity_factor_hori);
	intensity_factor_hori = NULL;
	free(avg_intensity_hori);
	avg_intensity_hori = NULL;

	// Done.
	return(TRUE);	
}

// Perform one single tracking cycle and return results:
//
// eyeResult  - Pointer to struct for return of all relevant tracking results.
// useHighGUI - FALSE == Only silently track and return data. TRUE = Also give visual feedback over OpenCV's GUI and accept hotkey input.
//
// Typical tracking cycle:
//
// 1. Grab a frame into the trackers internal memory buffers inside M-File via Screen('GetCaputuredImage',...);
// 2. Execute cvEyeTrackerExecuteTrackingCycle(<#PsychCVEyeResult * eyeResult#>,<#psych_bool useHighGUI#>);
// 3. Encode and return tracking result eyeResult to Matlab/Octave space.
// 4. After other Psychtoolbox M-File stuff, goto 1.
//
psych_bool cvEyeTrackerExecuteTrackingCycle(PsychCVEyeResult* eyeResult, psych_bool useHighGUI)
{
	char c;

	// Enable keyboard hotkeys and mouse functions as well as GUI
	// windows (non OpenGL, but OpenCV highGUI)?
	if (useHighGUI) {
		// Query keyboard the OpenCV way, with 1 millisecond timeout, ie., just poll:
		c=0;
		c=cvWaitKey(1);

		// Dispatch hot key press if any:
		if (c == 's') {
			// Save a snapshot of eye image and scene image:
			sprintf(eye_file, "eye%05d.bmp", image_no);
			sprintf(scene_file, "scene%05d.bmp", image_no);
			image_no++;
			cvSaveImage(eye_file, eye_image);
			cvSaveImage(scene_file, scene_image);
			printf("thres: %d\n", pupil_edge_thres);
		} else if (c == 'c') {
			// Toggle writeout of processed and visualized eye images:
			save_image = 1 - save_image;
			printf("save_image = %d\n", save_image);
		} else if (c == 'e') {
			// Toggle writeout of visualized ellipse images:
			save_ellipse = 1 - save_ellipse;
			printf("save_ellipse = %d\n", save_ellipse);
			if (save_ellipse == 1) {
				// Open ellipse data log if saving was enabled:
				Open_Ellipse_Log();
			} else {
				// Close ellipse data log if saving was disabled:
				fclose(ellipse_log);
			}
		}
	}
	
	// Perform image processing and tracking cycle:
	if (start_point.x == -1 && start_point.y == -1) {
		// Initial scan: Just init, don't track...
		Grab_Camera_Frames();
		if (gausswidth > 0) cvSmooth(eye_image, eye_image, CV_GAUSSIAN, gausswidth, gausswidth);
		Reduce_Line_Noise(eye_image);		
		cvThreshold(eye_image, threshold_image, pupil_edge_thres, 255, CV_THRESH_BINARY);
	}
	else {
		// Perform full cycle:
		process_image();
	}
	
	// Update GUI, if GUI is enabled:
	if (useHighGUI && (frame_number%1==0)) {
		// Update GUI, release temp image:
		Update_Gui_Windows(); 
	}
	else {
		// Only release temp image:
//		cvReleaseImage(&original_eye_image);
	}

	// Fill return struct with tracking results:
	eyeResult->gaze_x = gaze_point.x;
	eyeResult->gaze_y = gaze_point.y;
	eyeResult->pupil_x = pupil.x;
	eyeResult->pupil_y = pupil.y;
	eyeResult->cornea_x = corneal_reflection.x;
	eyeResult->cornea_y = corneal_reflection.y;
	eyeResult->validresult = (lost_frame_num > 0) ? FALSE : TRUE;
	eyeResult->trackcount = trackedframes++;
	eyeResult->timestamp = Time_Elapsed();

	// Done with tracking cycle:
	return(TRUE);
}

// Set the initial position of the center of the pupil: This just
// needs to be approximate, not exact -- A start value for the Starburst tracker:
void cvEyeTrackerSetPupilLocation(int px, int py)
{
	pupil.x = start_point.x = px;
	pupil.y = start_point.y = py;
	Draw_Cross(eye_image, pupil.x, pupil.y, 10, 10, Green);

	return;
}

// Add a eye --> scene gaze location calibration point at (px, py):
// (px, py) is the known/supposed gaze target, the current eye parameters
// are taken from last tracker cycle:
void cvEyeTrackerAddCalibrationPoint(int px, int py)
{
	Set_Calibration_Point(px, py);
	return;
}

// Recalculates calibration from current point correspondences, activates if
// enough points available.
void cvEyeTrackerRecalibrate(psych_bool resetCalib)
{
	if (resetCalib) {
		Zero_Calibration();
	}
	else {
		Activate_Calibration();
	}
	
	return;
}

// Set runtime operating parameter of the OpenEyes algorithm: Called mostly by PSYCHCVParameters()
void cvEyeTrackerSetParameters(int pupilEdgeThreshold, int starburstRays, int minFeatureCandidates, int corneaWindowSize, int edgeThreshold, int gaussWidth, double maxPupilEccentricity, double initialAngleSpread,
								double fanoutAngle1, double fanoutAngle2, int featuresPerRay, int specialFlags)
{
	pupil_edge_thres = pupilEdgeThreshold;
	rays = starburstRays;
	min_feature_candidates = minFeatureCandidates;
	cr_window_size = corneaWindowSize;
	edge_threshold = edgeThreshold;
	gausswidth = gaussWidth;
	maxeccentricity = maxPupilEccentricity;
	initial_angle_spread = initialAngleSpread;
	
	fanoutangle1 = fanoutAngle1;
	fanoutangle2 = fanoutAngle2;
	bouncerays = specialFlags;					// MK: If set to & 0x1, then Starburst will use bounced/reflected rays to add more features, otherwise not.
	features_per_ray = featuresPerRay;			// MK: Try to add up to features_per_ray features per Starburst beam.
	
	return;
}

// Set the additional filter constraints for RANSAC ellipse fitting:
void cvEyeTrackerSetRansacConstraints(double minDist, double maxDist, double minArea, double maxArea)
{
	min_feature_dist = minDist;
	max_feature_dist = maxDist;
	min_ellipse_area = minArea;
	max_ellipse_area = maxArea;

	return;
}

void cvEyeTrackerSetOverrideReferencePoint(int rx, int ry)
{
	corneal_reflection.x = rx;
	corneal_reflection.y = ry;

	// If values other than (-1,-1) were provided, disable auto-detection of corneal
	// reflection, use the provided values instead. Otherwise enable auto-detection...
	detectCornealReflection = (rx!=-1 && ry!=-1) ? FALSE : TRUE;
	
	return;
}

#endif
