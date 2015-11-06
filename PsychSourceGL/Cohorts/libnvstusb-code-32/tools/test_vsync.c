/* example program Copyright (C) 2010 Johann Baudy
 *
 * This program comes with ABSOLUTELY NO WARRANTY.
 * This is free software, and you are welcome to redistribute it
 * under certain conditions. See the file COPYING for details
 * */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <math.h>

#include <GL/glut.h>
#include <GL/glx.h>
#include <GL/glxext.h>

static PFNGLXGETVIDEOSYNCSGIPROC glXGetVideoSyncSGI = NULL;
static PFNGLXWAITVIDEOSYNCSGIPROC glXWaitVideoSyncSGI = NULL;
static PFNGLXSWAPINTERVALSGIPROC glXSwapIntervalSGI = NULL;
long i_sleep = 0;

void print_refresh_rate(void)
{
  static int i_it = 0;
  static uint64_t i_last = 0;
  static uint64_t i_first = 0;
  static uint64_t i_acc_var = 0;
  double f_mean, f_var;

  if(i_it == 0) {
    struct timespec s_tmp;
    clock_gettime(CLOCK_REALTIME, &s_tmp);
    i_first = (uint64_t)s_tmp.tv_sec*1000000+(uint64_t)s_tmp.tv_nsec/1000;
    i_last = i_first;
    f_mean = 0;
    f_var = 0;
  } else {
    struct timespec s_tmp;
    clock_gettime(CLOCK_REALTIME, &s_tmp);
    uint64_t i_new = (uint64_t)s_tmp.tv_sec*1000000+(uint64_t)s_tmp.tv_nsec/1000;
    f_mean = (double)(i_new-i_first)/(i_it);
    i_acc_var += pow((double)(i_new-i_last)-f_mean, 2);
    f_var = (double)sqrt(i_acc_var/i_it);
    i_last = i_new;

    if(i_it % 512 == 0) {
      printf("frame:%d (%0.2f s) mean: %f Hz (%0.2f us) sqrt(var): %0.2f us (%0.1f %%)\n",i_it,f_mean*i_it/1000000.0, 1000000/f_mean, f_mean, f_var, 100.0*f_var/f_mean);
    }
  }

  i_it++;
}


void drawNoImage() {
  static int i_current = 0;
  glClearColor(0.0, 0.0, 0.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT);

  glBegin(GL_QUADS);
  if (i_current) {
    glVertex2f(-1.0, -1.0);
    glVertex2f(-0.1, -1.0);
    glVertex2f(-0.1,  1.0);
    glVertex2f(-1.0,  1.0);
  } else {
    glVertex2f( 0.1, -1.0);
    glVertex2f( 1.0, -1.0);
    glVertex2f( 1.0,  1.0);
    glVertex2f( 0.1,  1.0);
  }
  glEnd();

  glFinish();

  if(!i_sleep)
  {
#if 0
    unsigned int i_before_count;
    unsigned int i_after_count;
    static unsigned int i_last_count;

    /* Waiting OpenGL sync */
    glXGetVideoSyncSGI(&i_before_count);


    glXWaitVideoSyncSGI(2, (i_before_count+1)%2, &i_after_count);
    if(i_after_count  != (i_before_count + 1)) {
      fprintf(stderr, "Count after error a:%d b:%d \n", i_after_count, i_before_count);
    }
    if(i_last_count != i_before_count) {
      fprintf(stderr, "Count last error a:%d b:%d \n", i_last_count, i_before_count);
    }
    i_last_count = i_after_count;
#else
    glXSwapIntervalSGI(1);
#endif
  } else {
    usleep(i_sleep);
  }
  print_refresh_rate();
  glutSwapBuffers();

  i_current = !i_current;
}

int main(int argc, char **argv) {

  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGB|GLUT_DOUBLE);

  glutInitWindowSize(512, 512);

  glutCreateWindow("3dv");
  glutIdleFunc(drawNoImage);

  if(argc < 2) {
    glXGetVideoSyncSGI = (PFNGLXGETVIDEOSYNCSGIPROC)glXGetProcAddress("glXGetVideoSyncSGI");
    glXWaitVideoSyncSGI = (PFNGLXWAITVIDEOSYNCSGIPROC)glXGetProcAddress("glXWaitVideoSyncSGI");
    if (NULL == glXWaitVideoSyncSGI) {
      fprintf(stderr, "nvstusb: GLX_SGI_video_sync not supported!\n");
      exit(EXIT_FAILURE);
    }

    if (NULL == glXGetVideoSyncSGI ) {
      fprintf(stderr, "nvstusb: GLX_SGI_video_sync not supported! (glXGetVideoSyncSGI)\n");
      exit(EXIT_FAILURE);
    }
  } else {
    i_sleep = 8133;
  }

  /* Swap interval */
  glXSwapIntervalSGI = (PFNGLXSWAPINTERVALSGIPROC)glXGetProcAddress("glXSwapIntervalSGI");

  if (NULL != glXSwapIntervalSGI) {
    fprintf(stderr, "nvstusb: forcing vsync\n");
  }
  glutMainLoop();

  return EXIT_SUCCESS;
}
