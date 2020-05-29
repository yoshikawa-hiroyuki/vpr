#ifndef _VPR_STRCONV_H_
#define _VPR_STRCONV_H_

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif
#include <string>

/* string converters */
inline wxString ConvSysToWx(const std::string& str) {
  using namespace std;
 
  wxString retStr;
  if ( str.empty() ) return retStr;
#ifdef WINDOWS
  retStr = wxString(str.c_str());
#else
  retStr = wxString::FromUTF8(str.c_str());
#endif
  return retStr;
}

inline std::string ConvWxToSys(const wxString& str) {
  using namespace std;

  string retStr;
  if ( str.IsEmpty() ) return retStr;
  retStr = str.mb_str();
  return retStr;
}

#endif // _VPR_STRCONV_H_
