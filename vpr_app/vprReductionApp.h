//
// vprReductionApp
//
#ifndef _VPR_REDUCTION_APP_H_
#define _VPR_REDUCTION_APP_H_

#include "wx/defs.h"
#include "vprReductionModule.h"

/* main app class */
class vprReductionApp : public wxApp {
public:
  vprReductionApp();
  bool initialize();
  vprReductionModule& getReductionModule() {return m_reducer;}
  bool isLoaded() const {return m_loaded;}
  
  bool OnInit(void);

private:
  vprReductionModule m_reducer;
  bool m_loaded;
};

#endif // _VPR_REDUCTION_APP_H_
