//
// vprReductionView
//
#ifdef __GNUG__
#pragma implementation
#pragma interface
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#ifdef __WXMAC__
#  ifdef DARWIN
#    include <OpenGL/glu.h>
#  else
#    include <glu.h>
#  endif
#else
#  include <GL/glu.h>
#endif

#include "vprReductionView.h"
#include "utilMath.h"
#include "vpr_strconv.h"

using namespace std;
using namespace VPR;


//----------------------------------------------------------------
// class vprReductionView
//----------------------------------------------------------------

BEGIN_EVENT_TABLE(vprReductionView, wxFrame)
  EVT_SPINCTRLDOUBLE(ReductView_PercentSpn, vprReductionView::OnPercentSpn)
  EVT_TEXT_ENTER(ReductView_PercentSpn, vprReductionView::OnPercentTxt)
  EVT_TEXT_ENTER(ReductView_TargTxt, vprReductionView::OnTargTxt)
  EVT_CLOSE(vprReductionView::OnClose)
  EVT_MENU(ReductView_Menu_FileOpen, vprReductionView::OnMenuFileOpen)
  EVT_MENU(ReductView_Menu_FileSave, vprReductionView::OnMenuFileSave)
  EVT_MENU(ReductView_Menu_FileExit, vprReductionView::OnMenuFileExit)
  EVT_MENU(ReductView_Menu_ViewFit, vprReductionView::OnMenuViewFit)
END_EVENT_TABLE()


/* constructor */

vprReductionView::vprReductionView(wxWindow *parent,
				   vprReductionModule* reducer)
: wxFrame(parent, -1, wxT("polygon reducer"),
	  wxDefaultPosition, wxSize(800, 600), 
	  wxDEFAULT_FRAME_STYLE|wxRESIZE_BORDER),
  m_status(NoData), m_nVtx(0), m_nFcs(0), m_percent(100), m_perDelta(1.0),
  m_reducer(reducer), m_canvas(NULL),
  m_percentSpn(NULL), m_targTxt(NULL), m_statTxt(NULL)  
{
  assert(m_reducer);

  m_nVtx = m_reducer->fpVPR_getNumVertex();
  m_nFcs = m_reducer->fpVPR_getNumFaces();
#if 0
  if ( m_nVtx < 3 || m_nFcs < 1 ) {
    wxMessageDialog dlg(NULL, wxT("Polygon Reducer: no valid data."),
			wxT("Error"));
    dlg.ShowModal();
    m_status = ErrOccured;
    return;
  }
#endif

  /* Title */
  string mod_name = CES::BaseName(string(reducer->m_modPath), string(""),
				  vprPath_getDelimChar());
  wxString ttl(wxT("polygon reducer ["));
  ttl += ConvSysToWx(mod_name);
  ttl += wxT("]");
  SetTitle(ttl);

  /* Layout of children */
  wxBoxSizer* topsizer = new wxBoxSizer(wxVERTICAL);
  wxBoxSizer* sizerH;
#if defined(WINDOWS)
  int *gl_attrib = NULL;
#else
  int gl_attrib[20] = {WX_GL_RGBA, WX_GL_MIN_RED, 1, WX_GL_MIN_GREEN, 1,
		       WX_GL_MIN_BLUE, 1, WX_GL_DEPTH_SIZE, 1,
		       WX_GL_DOUBLEBUFFER,
#  if defined(DARWIN)
		       GL_NONE
#  else
		       None
#  endif
  };
#endif
  m_canvas = new vprRV_GLCanvas(m_reducer, this, -1, wxDefaultPosition,
				wxDefaultSize, wxSUNKEN_BORDER,
				wxT("Polygon Reducer"), gl_attrib);
  assert(m_canvas);
  topsizer->Add(m_canvas, 1, wxEXPAND|wxALL, 3);

  sizerH = new wxBoxSizer(wxHORIZONTAL);
  topsizer->Add(sizerH, 0, wxEXPAND|wxALL, 3);

  sizerH->Add(new wxStaticText(this, -1, wxT("Faces(%)")),
              0, wxALIGN_LEFT|wxALL, 3);

  m_percentSpn = new wxSpinCtrlDouble(this, ReductView_PercentSpn, wxT("100"),
				      wxDefaultPosition, wxDefaultSize,
				      wxSP_VERTICAL|wxTE_PROCESS_ENTER,
				      m_perDelta, 100.0, 100.0, m_perDelta);
  assert(m_percentSpn);
  sizerH->Add(m_percentSpn, 0, wxEXPAND|wxALL, 3);

  sizerH->Add(10, 0);
  sizerH->Add(new wxStaticText(this, -1, wxT("#of Faces")),
              0, wxALIGN_LEFT|wxALL, 3);
  m_targTxt = new wxTextCtrl(this, ReductView_TargTxt,
			     wxString::Format(wxT("%d"), m_nFcs),
			     wxDefaultPosition, wxDefaultSize,
			     wxTE_PROCESS_ENTER);
  assert(m_targTxt);
  sizerH->Add(m_targTxt, 0, wxALL, 3);

  sizerH->Add(10, 0);
  m_statTxt = new wxTextCtrl(this, ReductView_StatTxt, wxT(""),
			     wxDefaultPosition, wxDefaultSize);
  assert(m_statTxt);
  sizerH->Add(m_statTxt, 1, wxEXPAND|wxALL, 3);

  /* Menubar */
  setupMenuBar();
  
  /* Fitting */
  m_canvas->FitView();

  /* Initial Status */
  UpdateStatusText();

  /* post process */
  SetSizer(topsizer);
  topsizer->Layout();
  Refresh(FALSE);
}


void vprReductionView::setupMenuBar() {
  wxMenuBar* pmb = new wxMenuBar;
  if ( ! pmb ) return;

  // File menu
  wxMenu* fileMenu = new wxMenu;
  if ( ! fileMenu ) return;
  fileMenu->Append(ReductView_Menu_FileOpen, wxT("Open ...\tCTRL+O"),
                   wxT("Open wavefront obj file"));
  fileMenu->Append(ReductView_Menu_FileSave, wxT("Save ...\tCTRL+W"),
                   wxT("Save wavefront obj file"));
  fileMenu->AppendSeparator();
  fileMenu->Append(ReductView_Menu_FileExit, wxT("Quit\tCTRL+Q"),
                   wxT("Quit app"));
  pmb->Append(fileMenu, wxT("File"));

  // View menu
  wxMenu* viewMenu = new wxMenu;
  if ( ! viewMenu ) return;
  viewMenu->Append(ReductView_Menu_ViewFit, wxT("Fit view\tSPACE"),
                   wxT("Fit view frustum to the object"));
  pmb->Append(viewMenu, wxT("View"));

  SetMenuBar(pmb);
}

void vprReductionView::UpdateStatusText() {
  int nVtx, nFcs;
  nVtx = m_reducer->fpVPR_getNumVertex();
  nFcs = m_reducer->fpVPR_getNumFaces();

  wxString status
    = wxString::Format(wxT("Original: %u[v]/%u[f]  Current: %u[v]/%u[f]"),
		       m_nVtx, m_nFcs, nVtx, nFcs);
  m_statTxt->SetValue(status);
}


/* event handlers */

void vprReductionView::OnMenuFileOpen(wxCommandEvent& event) {
  if ( m_status != NoData ) {
    wxMessageDialog dlg(this,
			wxT("Delete the current data and load another one,"
			    "\nAre you sure ?"),
			wxT("Open new obj file"),
			wxOK | wxCANCEL | wxCENTER );
    if ( dlg.ShowModal() != wxID_OK )
      return;
  }
  
  // prepare file dialog
  wxFileDialog fileDlg(this, wxT("select a Wavefront obj file to open"),
                       wxT(""), wxT(""), // default Dir / File
                       wxT("Obj file(*.obj)|*.obj|(*)|*"),
                       wxFD_OPEN);
  if ( fileDlg.ShowModal() != wxID_OK )
    return;

  // open an obj file
  wxString wpath = fileDlg.GetPath();
  vprPathIF wf;
  wf.fmt = 0; strncpy(wf.path, ConvWxToSys(wpath).c_str(), VRS_MAX_PATHLEN);
  if ( m_reducer->fpVPR_open(&wf) != VPR_SUCCEED ) {
    wxMessageDialog dlg2(NULL, wxT("Polygon Reducer: read obj file failed."),
			 wxT("Error"));
    dlg2.ShowModal();
    m_status = ErrOccured;
    return;
  }

  // reset params and status
  m_nVtx = m_reducer->fpVPR_getNumVertex();
  m_nFcs = m_reducer->fpVPR_getNumFaces();
  if ( m_nVtx < 3 || m_nFcs < 1 ) {
    wxMessageDialog dlg(NULL, wxT("Polygon Reducer: no valid data."),
			wxT("Error"));
    dlg.ShowModal();
    m_status = ErrOccured;
    return;
  }
  m_status = UnChanged;
  m_percent = 100;
  wxString ps = wxString::Format(wxT("%d"), m_percent);
  m_percentSpn->SetValue(ps);
  wxString ts = wxString::Format(wxT("%d"), m_nFcs);
  m_targTxt->SetValue(ts);
  UpdateStatusText();

  // fit
  m_canvas->FitView();
  Refresh(FALSE);
}

void vprReductionView::OnMenuFileSave(wxCommandEvent& event) {
  if ( m_status == NoData || m_status == ErrOccured ) {
    wxMessageDialog dlg(NULL, wxT("No valid polygon data, no need to save."),
			wxT("Save"));
    dlg.ShowModal();
    return;
  }
  if ( m_status == UnChanged ) {
    wxMessageDialog dlg(NULL, wxT("Polygon not reduced, no need to save."),
			wxT("Save"));
    dlg.ShowModal();
    return;
  }

  // prepare file dialog
  wxFileDialog fileDlg(this, wxT("select a Wavefront obj file to save"),
                       wxT(""), wxT(""), // default Dir / File
                       wxT("Obj file(*.obj)|*.obj|(*)|*"),
                       wxFD_SAVE);
  if ( fileDlg.ShowModal() != wxID_OK )
    return;

  // open an obj file
  wxString wpath = fileDlg.GetPath();
  vprPathIF wf;
  wf.fmt = 0; strncpy(wf.path, ConvWxToSys(wpath).c_str(), VRS_MAX_PATHLEN);
  if ( m_reducer->fpVPR_save(&wf) != VPR_SUCCEED ) {
    wxMessageDialog dlg2(NULL, wxT("Polygon Reducer: save obj file failed."),
			 wxT("Error"));
    dlg2.ShowModal();
    return;
  }

  return;
}

void vprReductionView::OnMenuFileExit(wxCommandEvent& event) {
  exit(0);
}

void vprReductionView::OnMenuViewFit(wxCommandEvent& event) {
  m_canvas->FitView();
}

void vprReductionView::OnPercentSpn(wxSpinDoubleEvent& event) {
  double val = m_percentSpn->GetValue();
  if ( val < 1.0 || val > 100.0 ) {
    wxString ps = wxString::Format(wxT("%d"), m_percent);
    m_percentSpn->SetValue(ps);
    return;
  }
  int target = (int)val;
  if ( target < (int)m_perDelta ) target = (int)m_perDelta;
  if ( target > 100 ) target = 100;
  m_percent = target;

  int tgtFcs = target * m_nFcs / 100;
  if ( tgtFcs == m_reducer->fpVPR_getNumFaces() ) return;
  m_reducer->fpVPR_decimate(tgtFcs);
  m_canvas->Refresh(FALSE);

  int nVtx, nFcs;
  nVtx = m_reducer->fpVPR_getNumVertex();
  nFcs = m_reducer->fpVPR_getNumFaces();
  if ( m_nVtx != nVtx || m_nFcs != nFcs )
    m_status = Decimated;
  else
    m_status = UnChanged;

  wxString ts = wxString::Format(wxT("%d"), nFcs);
  m_targTxt->SetValue(ts);
  UpdateStatusText();  
}

void vprReductionView::OnPercentTxt(wxCommandEvent& event) {
  wxString ps = event.GetString();
  if ( ps.IsEmpty() ) {
    wxString ps = wxString::Format(wxT("%d"), m_percent);
    m_percentSpn->SetValue(ps);
    return;
  }
  int target = atoi(ConvWxToSys(ps).c_str());
  if ( target < (int)m_perDelta || target > 100 ) {
    ps = wxString::Format(wxT("%d"), m_percent);
    m_percentSpn->SetValue(ps);
    return;
  }
  m_percent = target;

  int tgtFcs = target * m_nFcs / 100;
  if ( tgtFcs == m_reducer->fpVPR_getNumFaces() ) return;
  m_reducer->fpVPR_decimate(tgtFcs);
  m_canvas->Refresh(FALSE);

  int nVtx, nFcs;
  nVtx = m_reducer->fpVPR_getNumVertex();
  nFcs = m_reducer->fpVPR_getNumFaces();
  if ( m_nVtx != nVtx || m_nFcs != nFcs )
    m_status = Decimated;
  else
    m_status = UnChanged;

  wxString ts = wxString::Format(wxT("%d"), nFcs);
  m_targTxt->SetValue(ts);
  UpdateStatusText();
}

void vprReductionView::OnTargTxt(wxCommandEvent& event) {
  int nVtx, nFcs = m_reducer->fpVPR_getNumFaces();
  wxString ts = m_targTxt->GetValue();
  if ( ts.IsEmpty() ) {
    ts = wxString::Format(wxT("%d"), nFcs);
    m_targTxt->SetValue(ts);
    return;
  }
  int tgtFcs = atoi(ConvWxToSys(ts).c_str());
  if ( tgtFcs == nFcs ) return;
  if ( tgtFcs < 1 || tgtFcs > m_nFcs ) {
    ts = wxString::Format(wxT("%d"), nFcs);
    m_targTxt->SetValue(ts);
    return;
  }

  m_reducer->fpVPR_decimate(tgtFcs);
  m_canvas->Refresh(FALSE);

  nVtx = m_reducer->fpVPR_getNumVertex();
  nFcs = m_reducer->fpVPR_getNumFaces();
  if ( m_nVtx != nVtx || m_nFcs != nFcs )
    m_status = Decimated;
  else
    m_status = UnChanged;

  int tgtRatio = nFcs * 100 / m_nFcs;
  m_percentSpn->SetValue(wxString::Format(wxT("%d"), tgtRatio));
  UpdateStatusText();
}

void vprReductionView::OnClose(wxCloseEvent& event) {
  wxMessageDialog dlg(this, wxT("Are you sure to Quit ?"),
                            wxT("Quit"),
                            wxOK | wxCANCEL | wxCENTER );
  if ( dlg.ShowModal() == wxID_OK )
    exit(0);
}


//----------------------------------------------------------------
// class vprRV_GLCanvas
//----------------------------------------------------------------
// Notice:
//  definition of EVT_MOUSEWHEEL must be placed before EVT_MOUSE_EVENTS
BEGIN_EVENT_TABLE(vprRV_GLCanvas, wxGLCanvas)
  EVT_SIZE(vprRV_GLCanvas::OnSize)
  EVT_PAINT(vprRV_GLCanvas::OnPaint)
  EVT_ERASE_BACKGROUND(vprRV_GLCanvas::OnEraseBackground)
  EVT_MOUSEWHEEL(vprRV_GLCanvas::OnWheel)
  EVT_MOUSE_EVENTS(vprRV_GLCanvas::OnMouse)
  EVT_KEY_DOWN(vprRV_GLCanvas::OnKeyin)
END_EVENT_TABLE()


/* constructor/destructor */

vprRV_GLCanvas::vprRV_GLCanvas(vprReductionModule* reducer,
			       wxWindow *parent, wxWindowID id,
			       const wxPoint& pos, const wxSize& size,
			       long style, const wxString& name, int* attr)
  : wxGLCanvas(parent, id, pos, size, style, name, attr), p_reducer(reducer)
{
}

vprRV_GLCanvas::~vprRV_GLCanvas(void) {
}


/* event handlers */

void vprRV_GLCanvas::OnPaint(wxPaintEvent& event) {
  /* must always be here */
  wxPaintDC dc(this);

#ifndef __WXMOTIF__
  if ( ! GetContext() ) return;
#endif

  SetCurrent();

  /* initialize OpenGL */
  if ( info.do_init == TRUE ) {
    InitGL();
    info.do_init = FALSE;
  }

  double asp; int w, h;
  GetClientSize(&w, &h);
  if ( h < 1 ) asp = 1.0;
  else asp = (double)w / (double)h;
  
  /* view */
  glMatrixMode( GL_PROJECTION );
  glLoadIdentity();
  gluPerspective(info.fov, asp, 0.1, 99.9);
  glMatrixMode(GL_MODELVIEW);

  /* clear */
  glClearColor(0.753, 0.753, 0.753, 1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  /* transformations */
  GLfloat M[4][4];
  glLoadIdentity();
  glTranslatef(info.eye[0], info.eye[1], info.eye[2]);

  build_rotmatrix(M, info.quat);
  glMultMatrixf(&M[0][0]);

  glScalef(xform.m_S[0], xform.m_S[1], xform.m_S[2]);

  glTranslatef(xform.m_T[0], xform.m_T[1], xform.m_T[2]);

  /* draw object */
  if ( p_reducer ) {
    glDisable(GL_LIGHTING);
    p_reducer->fpVPR_drawGL();
    glEnable(GL_LIGHTING);
  }

  /* flush */
  glFlush();

  /* swap */
  SwapBuffers();
}

void vprRV_GLCanvas::OnSize(wxSizeEvent& event) {
  // this is also necessary to update the context on some platforms
  wxGLCanvas::OnSize(event);
    
  // set GL viewport (not called by wxGLCanvas::OnSize on all platforms...)
  int w, h;
  GetClientSize(&w, &h);
#ifndef __WXMOTIF__
  if ( GetContext() )
#endif
    {
      SetCurrent();
      glViewport(0, 0, (GLint) w, (GLint) h);
    }
}

void vprRV_GLCanvas::OnEraseBackground(wxEraseEvent& event) {
  /* Do nothing, to avoid flashing on MSW */
}

void vprRV_GLCanvas::OnMouse(wxMouseEvent& event) {
  if ( event.Entering() ) {
    SetFocus(); // need on windows to receive wheel events
  }
  wxSize sz(GetClientSize());
  if ( event.Dragging() ) {
    float dx = event.GetX() - info.beginx;
    float dy = event.GetY() - info.beginy;
    float h = -2 * info.eye[2] * tan((info.fov*0.5)*M_PI/180.0);
    if ( event.ControlDown() ) {
      info.eye[2] -= h * dy / sz.y;
    }
    else if ( event.ShiftDown() ) {
      info.eye[0] += h * dx / sz.y;
      info.eye[1] -= h * dy / sz.y;
    }
    else {
      /* drag in progress, simulate trackball */
      float spin_quat[4];
      trackball(spin_quat,
                (2.0*info.beginx - sz.x) / sz.x,
                (sz.y - 2.0*info.beginy) / sz.y,
                (2.0*event.GetX() - sz.x) / sz.x,
                (sz.y - 2.0*event.GetY()) / sz.y);
      add_quats( spin_quat, info.quat, info.quat );
    }

    /* orientation has changed, redraw mesh */
    Refresh(FALSE);
  }

  info.beginx = event.GetX();
  info.beginy = event.GetY();
}

void vprRV_GLCanvas::OnWheel(wxMouseEvent& event) {
  wxSize sz(GetClientSize());
  int wr = event.GetWheelRotation() > 0 ? 10 : -10;
  float h = -2 * info.eye[2] * tan((info.fov*0.5)*M_PI/180.0);
  info.eye[2] -= h * wr / sz.y;
  Refresh(FALSE);
  info.beginx = event.GetX();
  info.beginy = event.GetY();
}

void vprRV_GLCanvas::OnKeyin(wxKeyEvent& event) {
  int kc = event.GetKeyCode();
  if ( kc == WXK_SPACE ) {
    info.reset();
    FitView();
    Refresh(FALSE);
  }
  else if ( kc == WXK_ESCAPE ) {
    wxMessageDialog dlg(NULL, wxT("Are you sure to Quit ?"),
			wxT("Quit"),
			wxOK | wxCANCEL | wxCENTER );
    if ( dlg.ShowModal() == wxID_OK )
    exit(0);
  }
}


void vprRV_GLCanvas::InitGL(void) {
  GLfloat light0_pos[4]   = { 0.0, 0.0, -50.0, 0.0 };
  GLfloat light0_color[4] = { 0.56, 0.14, 0.42, 1.0 };
  GLfloat light1_pos[4]   = { 0.0, 0.0,  50.0, 0.0 };
  GLfloat light1_color[4] = { 1.0, 1.0, 1.0, 1.0 };

  /* remove back faces */
  glDisable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_NORMALIZE);
  
  /* speedups */
  glEnable(GL_DITHER);
  glShadeModel(GL_SMOOTH);
  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
  glHint(GL_POLYGON_SMOOTH_HINT, GL_FASTEST);

  /* light */
  glLightfv(GL_LIGHT0, GL_POSITION, light0_pos);
  glLightfv(GL_LIGHT0, GL_DIFFUSE,  light0_color);  
  glLightfv(GL_LIGHT1, GL_POSITION, light1_pos);
  glLightfv(GL_LIGHT1, GL_DIFFUSE,  light1_color);
  glEnable(GL_LIGHT0);
  glEnable(GL_LIGHT1);
  glEnable(GL_LIGHTING);
    
  glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
  glEnable(GL_COLOR_MATERIAL);

  /* Polygon Offset */
  glPolygonOffset(1.0, 0.000001);
  glEnable(GL_POLYGON_OFFSET_FILL);

  glLineWidth(0.5f);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_LINE_SMOOTH);
}

void vprRV_GLCanvas::FitView() {
  if ( ! p_reducer ) return;

  float bbox[2][3];
  if ( p_reducer->fpVPR_getBBox(bbox) != VPR_SUCCEED ) return;

  CES::Vec3<float> center
    = (CES::Vec3<float>(bbox[0]) + CES::Vec3<float>(bbox[1])) * 0.5f;
  float radius
    = (CES::Vec3<float>(bbox[1]) - CES::Vec3<float>(bbox[0])).Length() * 0.5f;

  info.reset();
  if ( radius > 1e-6 ) {
    float R = fabs(info.eye[2] * tan((info.fov*0.5)*M_PI/180.0));
    xform.m_S[0] = xform.m_S[1] = xform.m_S[2] = R / radius;
  } else {
    xform.m_S[0] = xform.m_S[1] = xform.m_S[2] = 1.0f;
  }

  xform.m_T[0] = -center[0];
  xform.m_T[1] = -center[1];
  xform.m_T[2] = -center[2];

  Refresh(FALSE);
}
