//
// vprInterface
//
#ifndef _VPR_INTERFACE_H_
#define _VPR_INTERFACE_H_

#ifdef WIN32
#define VPR_API(type) type __cdecl
#else // !WIN32
#define VPR_API(type) type
#endif // WIN32

#define VPR_VERSION "1.0"

#define VPR_SUCCEED 1
#define VPR_FAILED  0

#define VRS_MAX_PATHLEN 512


#ifdef __cplusplus
extern "C" {
#endif

  /* Path interface structure */

  typedef struct {
    // file format: 0=SMF, other=not support
    int fmt;
    char path[VRS_MAX_PATHLEN];
  } vprPathIF;


  /* Prototypes for the functions the library needs to provide */

  VPR_API(void) VPR_getVersion(char ver[VRS_MAX_PATHLEN]);
  VPR_API(int)  VPR_open(vprPathIF* pathIF);
  VPR_API(int)  VPR_save(vprPathIF* pathIF);
  VPR_API(int)  VPR_getNumVertex();
  VPR_API(int)  VPR_getNumFaces();
  VPR_API(int)  VPR_getBBox(float bbox[2][3]);
  VPR_API(int)  VPR_decimate(int targNumFaces);
  VPR_API(void) VPR_drawGL();
  VPR_API(void) VPR_getLastError(char msg[VRS_MAX_PATHLEN]);

#ifdef __cplusplus
}
#endif

#endif //  _VPR_INTERFACE_H_
