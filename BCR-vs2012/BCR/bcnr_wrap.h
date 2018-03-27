#ifndef BCNR_WRAP_H
#define BCNR_WRAP_H

#include <highgui.h> 
#include <cv.h>

#ifdef __cplusplus
extern "C" {
#endif

int init_all ( char * cnn_bin,
				char * cnn_plane_bin,
				char * bc_bin,
				char * mode_path,
				char * cascade_bin );
int recognise_from_pic ( char * pic_file, char * s_result );
int recognise_from_IplImage ( IplImage * src, char * s_result );

int release_all ();


#ifdef __cplusplus
}
#endif

#endif
