//
// vprPathUtil
//
#ifndef _VPR_PATH_UTIL_H_
#define _VPR_PATH_UTIL_H_

#include "utilPath.h"


namespace VPR {
  inline char vprPath_getDelimChar() {
#ifdef WIN32
    return '\\';
#else
    return '/';
#endif
  }

  inline void vprPath_adjustDelim(std::string& path) {
#ifdef WIN32
    const char newDelim = '\\';
    const char oldDelim = '/';
#else
    const char oldDelim = '\\';
    const char newDelim = '/';
#endif
    size_t pathLen = path.size();
    for ( register size_t i = 0; i < pathLen; i++ ) {
      if ( path[i] == oldDelim )
	path[i] = newDelim;
    } // end of for(i)
  }

  inline bool vprPath_hasDrive(const std::string& path) {
    if ( path.size() < 2 ) return false;
    char x = path[0];
    if ( ((x >= 'A' && x <= 'Z' ) || (x >= 'a' && x <= 'z')) &&
	 path[1] == ':' )
      return true;
    return false;
  }

  inline std::string vprPath_emitDrive(std::string& path) {
    // returns drive (ex. 'C:')
    if ( ! vprPath_hasDrive(path) ) return std::string();
    std::string driveStr = path.substr(0, 2);
    path = path.substr(2);
    return driveStr;
  }

  inline bool vprPath_isAbsolute(const std::string& path) {
    std::string xpath(path);
    vprPath_emitDrive(xpath);
    return (xpath[0] == vprPath_getDelimChar());
  }

  inline std::string vprPath_concat(const std::string& path1,
				    const std::string& path2) {
    std::string xpath;
    if ( ! path1.empty() ) {
      xpath = path1;
      xpath += vprPath_getDelimChar();
    }
    xpath += path2;
    return xpath;
  }

  inline std::string vprPath_normalize(const std::string& path) {
    std::string xpath(path);
    vprPath_adjustDelim(xpath);
    std::string driveStr = vprPath_emitDrive(xpath);
    xpath = CES::OmitDots(xpath, vprPath_getDelimChar());
    if ( ! driveStr.empty() )
      xpath = driveStr + xpath;
    return xpath;
  }
};

#endif // _VPR_PATH_UTIL_H_


