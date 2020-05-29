#ifdef __GNUG__
#pragma implementation
#pragma interface
#endif
#include "wx/wxprec.h"
#ifndef WX_PRECOMP
#include "wx/wx.h"
#include "wx/utils.h"
#endif

#include "vprReductionApp.h"
#include "vprReductionView.h"
#include "vpr_strconv.h"

IMPLEMENT_APP(vprReductionApp)


void usage(const char* app) {
  printf("usage: %s [objfile.obj]\n", app);
}

vprReductionApp::vprReductionApp() : m_reducer(), m_loaded(false) {}


bool vprReductionApp::OnInit(void) {
  // initialize reducer with vpr_qslim module
  wxString progDir = wxPathOnly(argv[0]);
#if defined(DARWIN)
  wxString mod_file(wxT("vpr_qslim.dylib"));
  wxString mod_path = progDir + wxT("/") + mod_file;
#elif defined(WINDOWS)
  wxString mod_file(wxT("vpr_qslim.dll"));
  wxString mod_path = progDir + wxT("\\") + mod_file;
#else
  wxString mod_file(wxT("vpr_qslim.so"));
  wxString mod_path = progDir + wxT("/") + mod_file;
  //wxString mod_path = mod_file;
  wxString ldpath;
  if ( wxGetEnv(wxT("LD_LIBRARY_PATH"), &ldpath) )
    ldpath = ldpath + wxT(":") + progDir;
  else
    ldpath = progDir;
  wxSetEnv(wxT("LD_LIBRARY_PATH"), ldpath);
#endif

  if ( ! m_reducer.initialize(ConvWxToSys(mod_path).c_str()) ) {
    wxMessageDialog dlg(NULL, wxT("Polygon reducer: initialize failed."),
			mod_path);
    dlg.ShowModal();
    //return FALSE;
    exit(1);
  }

  // load obj file if specified argv
  wxString infile;
  if ( argc > 1 ) {
    infile = argv[1];
  }
  if ( ! infile.empty() ) {
    vprPathIF wf;
    wf.fmt = 0; strncpy(wf.path, ConvWxToSys(infile).c_str(), VRS_MAX_PATHLEN);
    if ( m_reducer.fpVPR_open(&wf) != VPR_SUCCEED ) {
      wxMessageDialog dlg(NULL, wxT("Polygon Reducer: read obj file failed."),
			  wxT("Error"));
      dlg.ShowModal();
    }
    m_loaded = true;
  }

  vprReductionView* view = new vprReductionView(NULL, &m_reducer);
  SetTopWindow(view);
  view->Show(TRUE);

  return TRUE;
}
