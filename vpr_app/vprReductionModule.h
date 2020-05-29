//
// vprReductionModule
//
#ifndef _VPR_REDUCTION_MODULE_H_
#define _VPR_REDUCTION_MODULE_H_

#ifdef WINDOWS
#include <windows.h>
#endif

#include <cstdio>
#include <string>

#ifndef WINDOWS
extern "C" {
#include <dlfcn.h>
}
#endif // !WINDOWS

#include "vprInterface.h"
#include "vprPathUtil.h"

struct vprReductionModule {
#ifdef WINDOWS
  void (__cdecl *fpVPR_getVersion)(char*);
  int  (__cdecl *fpVPR_open)(vprPathIF*);
  int  (__cdecl *fpVPR_save)(vprPathIF*);
  int  (__cdecl *fpVPR_getNumVertex)();
  int  (__cdecl *fpVPR_getNumFaces)();
  int  (__cdecl *fpVPR_getBBox)(float[2][3]);
  int  (__cdecl *fpVPR_decimate)(int);
  void (__cdecl *fpVPR_drawGL)();
  void (__cdecl *fpVPR_getLastError)(char*);
  HINSTANCE hMod;
#else
  void (*fpVPR_getVersion)(char*);
  int  (*fpVPR_open)(vprPathIF*);
  int  (*fpVPR_save)(vprPathIF*);
  int  (*fpVPR_getNumVertex)();
  int  (*fpVPR_getNumFaces)();
  int  (*fpVPR_getBBox)(float[2][3]);
  int  (*fpVPR_decimate)(int);
  void (*fpVPR_drawGL)();
  void (*fpVPR_getLastError)(char*);
  void* hMod;
#endif
  std::string m_modPath;

  // default constructor
  vprReductionModule() :
    hMod(NULL), fpVPR_getVersion(NULL), fpVPR_open(NULL), fpVPR_save(NULL),
    fpVPR_getNumVertex(NULL), fpVPR_getNumFaces(NULL), fpVPR_getBBox(NULL),
    fpVPR_decimate(NULL), fpVPR_drawGL(NULL), fpVPR_getLastError(NULL) {
  }

  // destructor
  ~vprReductionModule() {
    shutdown();
  }

  // initializer
  bool initialize(const char* xfmod) {
#ifdef LINUX
    std::string fmod = xfmod;
#else
    std::string fmod = VPR::vprPath_normalize(xfmod);
#endif
    if ( hMod ) {
      if ( m_modPath == fmod ) return true;
      shutdown();
    }
    if ( fmod.empty() ) return false;
#ifdef WINDOWS
    wchar_t widestr[VRS_MAX_PATHLEN] = {0x00};
    MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED,
		        fmod.c_str(), fmod.size(), widestr, VRS_MAX_PATHLEN);
    hMod = ::LoadLibraryEx(widestr, NULL, 0);
    if ( ! hMod ) return false;
    fpVPR_getVersion
      = (void (__cdecl*)(char*)) ::GetProcAddress(hMod, "VPR_getVersion");
    fpVPR_open
      = (int (__cdecl*)(vprPathIF*)) ::GetProcAddress(hMod, "VPR_open");
    fpVPR_save
      = (int (__cdecl*)(vprPathIF*)) ::GetProcAddress(hMod, "VPR_save");
    fpVPR_getNumVertex
      = (int (__cdecl*)()) ::GetProcAddress(hMod, "VPR_getNumVertex");
    fpVPR_getNumFaces
      = (int (__cdecl*)()) ::GetProcAddress(hMod, "VPR_getNumFaces");
    fpVPR_getBBox
      = (int (__cdecl*)(float[2][3])) ::GetProcAddress(hMod, "VPR_getBBox");
    fpVPR_decimate
      = (int (__cdecl*)(int)) ::GetProcAddress(hMod, "VPR_decimate");
    fpVPR_drawGL
      = (void (__cdecl*)()) ::GetProcAddress(hMod, "VPR_drawGL");
    fpVPR_getLastError
      = (void (__cdecl*)(char*)) ::GetProcAddress(hMod, "VPR_getLastError");
#else
    hMod = dlopen(fmod.c_str(), RTLD_LAZY);
    if ( ! hMod ) return false;
    fpVPR_getVersion = (void (*)(char*))dlsym(hMod, "VPR_getVersion");
    fpVPR_open = (int (*)(vprPathIF*))dlsym(hMod, "VPR_open");
    fpVPR_save = (int (*)(vprPathIF*))dlsym(hMod, "VPR_save");
    fpVPR_getNumVertex = (int (*)())dlsym(hMod, "VPR_getNumVertex");
    fpVPR_getNumFaces = (int (*)())dlsym(hMod, "VPR_getNumFaces");
    fpVPR_getBBox = (int (*)(float[2][3]))dlsym(hMod, "VPR_getBBox");
    fpVPR_decimate = (int (*)(int))dlsym(hMod, "VPR_decimate");
    fpVPR_drawGL = (void (*)())dlsym(hMod, "VPR_drawGL");
    fpVPR_getLastError = (void (*)(char*))dlsym(hMod, "VPR_getLastError");
#endif

    if ( ! fpVPR_getVersion || ! fpVPR_open || ! fpVPR_save ||
	 ! fpVPR_getNumVertex || ! fpVPR_getNumFaces || ! fpVPR_getBBox ||
	 ! fpVPR_decimate || ! fpVPR_drawGL || ! fpVPR_getLastError ) {
      shutdown();
      return false;
    }

    char vprVer[VRS_MAX_PATHLEN];
    fpVPR_getVersion(vprVer);
    if ( strncmp(vprVer, VPR_VERSION, VRS_MAX_PATHLEN) ) {
      shutdown();
      return false;
    }

    m_modPath = fmod;
    return true;
  }

  // finalizer
  void shutdown() {
    if ( hMod ) {
#ifdef WINDOWS
      ::FreeLibrary(hMod);
#else
      dlclose(hMod);
#endif
      hMod = NULL;
      fpVPR_getVersion = NULL;
      fpVPR_open = NULL;
      fpVPR_save = NULL;
      fpVPR_getNumVertex = NULL;
      fpVPR_getNumFaces = NULL;
      fpVPR_getBBox = NULL;
      fpVPR_decimate = NULL;
      fpVPR_drawGL = NULL;
      fpVPR_getLastError = NULL;
    }
  }
};

#endif // _VPR_REDUCTION_MODULE_H_
