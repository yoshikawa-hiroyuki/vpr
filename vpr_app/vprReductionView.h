//
// vprReductionView
//
#ifndef _VPR_REDUCTION_VIEW_H_
#define _VPR_REDUCTION_VIEW_H_

#include "wx/defs.h"
#include "wx/dialog.h"
#include "wx/textctrl.h"
#include "wx/spinctrl.h"
#include "wx/glcanvas.h"

#include "vprReductionModule.h"
#include "trackball.h"


namespace VPR {
  // control ids
  enum {ReductView_StatTxt = 10000,
	ReductView_PercentSpn,
	ReductView_TargTxt,
	ReductView_Menu_FileOpen,
	ReductView_Menu_FileSave,
	ReductView_Menu_FileExit,
	ReductView_Menu_ViewReset,
  };
};


/* information needed to display  */

struct view_info {
  int do_init;
  float beginx,beginy;  /* position of mouse */
  float quat[4];        /* orientation of object */
  float fov;            /* field of view in degrees */
  float eye[3];
  void reset() {
    do_init = TRUE; beginx = beginy = 0;
    fov = 45; eye[0] = eye[1] = 0.f; eye[2] = -15.f;
    trackball(quat, 0.0, 0.0, 0.0, 0.0);
  }
  view_info() {reset();}
};

struct model_xform {
  float m_T[3];
  float m_S[3];
  model_xform() {
    m_T[0] = m_T[1] = m_T[2] = 0.f;
    m_S[0] = m_S[1] = m_S[2] = 1.f;
  }
};

/* pre-definition */
class vprRV_GLCanvas;


//----------------------------------------------------------------
// class vprReductionView
//----------------------------------------------------------------
class vprReductionView: public wxFrame {
public:
  // view status
  enum StatusType {NoData, UnChanged, Decimated, ErrOccured};

  vprReductionView(wxWindow *parent, vprReductionModule* reducer);

  void OnPercentSpn(wxSpinDoubleEvent& event);
  void OnPercentTxt(wxCommandEvent& event);
  void OnTargTxt(wxCommandEvent& event);
  void OnMenuFileOpen(wxCommandEvent& event);
  void OnMenuFileSave(wxCommandEvent& event);
  void OnMenuFileExit(wxCommandEvent& event);
  void OnMenuViewReset(wxCommandEvent& event);
  void OnClose(wxCloseEvent& event);
    
  void SetCanvas(vprRV_GLCanvas *canvas ) {m_canvas = canvas;}
  vprRV_GLCanvas *GetCanvas() {return m_canvas;}
  void UpdateStatusText();
  StatusType GetStatus() const {return m_status;}

private:
  vprReductionModule *m_reducer;
  vprRV_GLCanvas     *m_canvas;
  wxSpinCtrlDouble   *m_percentSpn;
  wxTextCtrl         *m_targTxt;
  wxTextCtrl         *m_statTxt;
  StatusType          m_status;
  int                 m_nVtx, m_nFcs, m_percent;
  double              m_perDelta;

  void setupMenuBar();

  DECLARE_EVENT_TABLE()
};


//----------------------------------------------------------------
// class vprRV_GLCanvas
//----------------------------------------------------------------
class vprRV_GLCanvas: public wxGLCanvas {
public:
  vprRV_GLCanvas(vprReductionModule* reducer,
		 wxWindow *parent, const wxWindowID id = -1,
		 const wxPoint& pos = wxDefaultPosition,
		 const wxSize& size = wxDefaultSize, long style = 0,
		 const wxString& name = wxT("vxg Polygon Reducer"),
		 int* attr =NULL);
  virtual ~vprRV_GLCanvas(void);

  void OnPaint(wxPaintEvent& event);
  void OnSize(wxSizeEvent& event);
  void OnEraseBackground(wxEraseEvent& event);
  void OnMouse(wxMouseEvent& event);
  void OnWheel(wxMouseEvent& event);
  void OnKeyin(wxKeyEvent& event);
  
  void InitGL(void);
  void ResetView();

  view_info   info;
  model_xform xform;

private:
  vprReductionModule* p_reducer;

  DECLARE_EVENT_TABLE()
};

#endif // _VPR_REDUCTION_VIEW_H_
