//
// vpr_qslim
//
#ifdef WIN32
#include <windows.h>

BOOL WINAPI DllMain(HINSTANCE hDLL, DWORD dwReason, LPVOID lpReserved) {
  return TRUE;
}
#endif // WIN32

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else // __APPLE__
#include <GL/gl.h>
#include <GL/glu.h>
#endif // __APPLE__


#include "vprInterface.h"

#include "stdmix.h"
#include "mixio.h"
#include "MxQSlim.h"
#include "MxSMF.h"
#include "MxGeom3D.h"

typedef MxDynBlock<MxPairContraction> QSlimLog;


/*******************************
  static datas
*******************************/

static MxSMFReader* s_smf;
static MxStdModel* s_mdl;
static MxQSlim* s_slim;
static MxEdgeQSlim* s_eslim;
static QSlimLog* s_history;
static MxDynBlock<MxEdge>* s_target_edges;
static unsigned s_orig_vcnt, s_orig_fcnt;
static char err_buff[VRS_MAX_PATHLEN];

static bool s_drawWire = true;
static GLfloat s_bodyColor[] = {1.f, 1.f, 1.f, 1.f};
static GLfloat s_wireColor[] = {0.8f, 0.2f, 0.2f, 0.5f};

/*******************************
  static utils
*******************************/

static void slim_undo() {
  if ( ! s_history || ! s_eslim ) return;
  MxPairContraction& conx = s_history->drop();
  s_eslim->apply_expansion(conx);
}

static void slim_to_target(uint target) {
  if ( ! s_mdl || ! s_slim ) return;
  if ( target > s_mdl->face_count() )
    target = s_mdl->face_count();

  if ( target < s_slim->valid_faces )
    s_slim->decimate(target);
  else if( target > s_slim->valid_faces && s_history ) {
    while ( s_slim->valid_faces < target && s_history->length() > 0 )
      slim_undo();
  }
}

static bool qslim_smf_hook(char *op, int, char *argv[], MxStdModel& m) {
  if( streq(op, "e") ) {
    if( ! s_target_edges )
      s_target_edges = new MxDynBlock<MxEdge>(m.vert_count() * 3);
    MxEdge& e = s_target_edges->add();
    e.v1 = atoi(argv[0]) - 1;
    e.v2 = atoi(argv[1]) - 1;
    return true;
  }
  return false;
}

static void slim_history_callback(const MxPairContraction& conx, float cost) {
  QSlimLog *history = s_history;
  if ( history ) history->add(conx);
}


/*******************************
  vpr interface functions
*******************************/
extern "C" {

void VPR_getVersion(char ver[VRS_MAX_PATHLEN]) {
  strncpy(ver, VPR_VERSION, VRS_MAX_PATHLEN);
}

int VPR_open(vprPathIF* pathIF) {
  if ( ! pathIF ) {
    strncpy(err_buff, "VPR_open: null argument.", VRS_MAX_PATHLEN);
    return VPR_FAILED;
  }
  if ( pathIF->fmt != 0 ) {
    strncpy(err_buff, "VPR_open: format not supported.", VRS_MAX_PATHLEN);
    return VPR_FAILED;
  }

  // clean and alloc
  if ( s_smf ) {delete s_smf; s_smf = NULL;}
  if ( s_mdl ) {delete s_mdl; s_mdl = NULL;}
  if ( s_slim ) {delete s_slim; s_slim = NULL;}
  if ( s_eslim ) {s_eslim = NULL;}
  if ( s_history ) {delete s_history; s_history = NULL;}
  if ( s_target_edges ) {delete s_target_edges; s_target_edges = NULL;}

  s_smf = new MxSMFReader();
  s_smf->unparsed_hook = qslim_smf_hook;
  s_mdl = new MxStdModel(100, 100);
  s_orig_vcnt = s_orig_fcnt = 0;

  // read the file
  ifstream in(pathIF->path);
  if ( ! in.good() ) {
    strncpy(err_buff, "VPR_open: can't open file.", VRS_MAX_PATHLEN);
    return VPR_FAILED;
  }
  s_smf->read(in, s_mdl);
  in.close();

  // setup QSlim
#ifdef WIN32
  if ( s_mdl->normal_binding() == MX_UNBOUND ) {
    s_mdl->normal_binding(MX_PERFACE);
    s_mdl->synthesize_normals();
  }
#else
  s_mdl->color_binding(MX_UNBOUND);
  s_mdl->normal_binding(MX_UNBOUND);
  s_mdl->texcoord_binding(MX_UNBOUND);
#endif

  s_slim = s_eslim = new MxEdgeQSlim(*s_mdl);
  s_slim->placement_policy = MX_PLACE_OPTIMAL;
  s_slim->boundary_weight = 1000.0;
  s_slim->weighting_policy = MX_WEIGHT_AREA;
  s_slim->compactness_ratio = 0.0;
  s_slim->meshing_penalty = 1.0;
  s_slim->will_join_only = false;

  if ( s_eslim && s_target_edges ) {
    s_eslim->initialize(*s_target_edges, s_target_edges->length());
  }
  else
    s_slim->initialize();

  s_history = new QSlimLog(100);
  s_eslim->contraction_callback = slim_history_callback;

  s_orig_vcnt = s_mdl->vert_count();
  s_orig_fcnt = s_mdl->face_count();

  return VPR_SUCCEED;
}


int VPR_save(vprPathIF* pathIF) {
  if ( ! pathIF ) {
    strncpy(err_buff, "VPR_save: null argument.", VRS_MAX_PATHLEN);
    return VPR_FAILED;
  }
  if ( pathIF->fmt != 0 ) {
    strncpy(err_buff, "VPR_save: format not supported.", VRS_MAX_PATHLEN);
    return VPR_FAILED;
  }
  if ( ! s_mdl ) {
    strncpy(err_buff, "VPR_save: no valid model to save.", VRS_MAX_PATHLEN);
    return VPR_FAILED;
  }

  // save to the file
  ofstream out(pathIF->path);
  if ( ! out.good() ) {
    strncpy(err_buff, "VPR_save: can't open file.", VRS_MAX_PATHLEN);
    return VPR_FAILED;
  }
  for ( uint i = 0; i < s_mdl->vert_count(); i++ )
    if ( s_mdl->vertex_is_valid(i) && s_mdl->neighbors(i).length() == 0 )
      s_mdl->vertex_mark_invalid(i);
  s_mdl->compact_vertices();

  MxSMFWriter writer;
  writer.write(out, *s_mdl);

  return VPR_SUCCEED;
}


int VPR_getNumVertex() {
  if ( ! s_slim ) {
    strncpy(err_buff, "VPR_getNumVertex: no valid model.", VRS_MAX_PATHLEN);
    return VPR_FAILED;
  }
  return s_slim->valid_verts;
}


int VPR_getNumFaces() {
  if ( ! s_slim ) {
    strncpy(err_buff, "VPR_getNumFaces: no valid model.", VRS_MAX_PATHLEN);
    return VPR_FAILED;
  }
  return s_slim->valid_faces;
}


int VPR_getBBox(float bbox[2][3]) {
  if ( ! s_slim ) {
    strncpy(err_buff, "VPR_getBBox: no valid model.", VRS_MAX_PATHLEN);
    return VPR_FAILED;
  }

  MxBounds bounds;
  for ( MxVertexID v = 0; v < s_mdl->vert_count(); v++ )
    bounds.add_point(s_mdl->vertex(v));
  bounds.complete();

  bbox[0][0] = bounds.min[0];
  bbox[0][1] = bounds.min[1];
  bbox[0][2] = bounds.min[2];
  bbox[1][0] = bounds.max[0];
  bbox[1][1] = bounds.max[1];
  bbox[1][2] = bounds.max[2];

  return VPR_SUCCEED;
}


int VPR_decimate(int targNumFaces) {
  if ( ! s_mdl || ! s_slim ) {
    strncpy(err_buff, "VPR_decimate: no valid model.", VRS_MAX_PATHLEN);
    return VPR_FAILED;
  }

  int target = targNumFaces;
  if ( target < 1 ) target = 1;
  slim_to_target(target);

  return VPR_SUCCEED;
}


void VPR_drawGL() {
  if ( ! s_mdl ) return;

  glColor4fv(s_bodyColor);
  mx_render_model(*s_mdl);

  if ( s_drawWire ) {
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glDepthMask(GL_FALSE);
    glColor4fv(s_wireColor);

#if 1
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    mx_render_model(*s_mdl);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
#else
    mx_draw_wireframe(*s_mdl);
#endif

    glEnable(GL_LIGHTING);
    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
  }
}


void VPR_getLastError(char msg[VRS_MAX_PATHLEN]) {
  strncpy(msg, err_buff, VRS_MAX_PATHLEN);
}

} // end of "C"
