// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

/// \file
/// \brief OpenCV algorithm registration implementation

#include <ocv/kwiver_algo_ocv_plugin_export.h>
#include <vital/algo/algorithm_factory.h>

#include <opencv2/opencv_modules.hpp>
#ifdef HAVE_OPENCV_NONFREE
#include <opencv2/nonfree/nonfree.hpp>
#endif

#include <ocv/analyze_tracks.h>
#include <ocv/detect_features_AGAST.h>
#include <ocv/detect_features_FAST.h>
#include <ocv/detect_features_GFTT.h>
#include <ocv/detect_features_MSD.h>
#include <ocv/detect_features_MSER.h>
#include <ocv/detect_features_simple_blob.h>
#include <ocv/detect_features_STAR.h>
#include <ocv/draw_detected_object_set.h>
#include <ocv/draw_tracks.h>
#include <ocv/estimate_fundamental_matrix.h>
#include <ocv/estimate_homography.h>
#include <ocv/estimate_pnp.h>
#include <ocv/resection_camera.h>
#include <ocv/extract_descriptors_BRIEF.h>
#include <ocv/extract_descriptors_DAISY.h>
#include <ocv/extract_descriptors_FREAK.h>
#include <ocv/extract_descriptors_LATCH.h>
#include <ocv/extract_descriptors_LUCID.h>
#include <ocv/feature_detect_extract_BRISK.h>
#include <ocv/feature_detect_extract_ORB.h>
#include <ocv/feature_detect_extract_SIFT.h>
#include <ocv/feature_detect_extract_SURF.h>
#include <ocv/image_io.h>
#include <ocv/inpaint.h>
#include <ocv/match_features_bruteforce.h>
#include <ocv/match_features_flannbased.h>
#include <ocv/merge_images.h>
#include <ocv/hough_circle_detector.h>
#include <ocv/refine_detections_write_to_disk.h>
#include <ocv/split_image.h>
#include <ocv/track_features_klt.h>
#include <ocv/detect_motion_3frame_differencing.h>
#include <ocv/detect_motion_mog2.h>
#include <ocv/detect_heat_map.h>

namespace kwiver {
namespace arrows {
namespace ocv {

extern "C"
KWIVER_ALGO_OCV_PLUGIN_EXPORT
void
register_factories( kwiver::vital::plugin_loader& vpm )
{
  kwiver::vital::algorithm_registrar reg( vpm, "arrows.ocv" );

  if (reg.is_module_loaded())
  {
    return;
  }

#if defined(HAVE_OPENCV_NONFREE)
  cv::initModule_nonfree();
#endif

  reg.register_algorithm< analyze_tracks >();
  reg.register_algorithm< draw_tracks >();
  reg.register_algorithm< estimate_fundamental_matrix >();
  reg.register_algorithm< estimate_homography >();
  reg.register_algorithm< image_io >();
  reg.register_algorithm< inpaint >();
  reg.register_algorithm< draw_detected_object_set >();

  reg.register_algorithm< detect_features_BRISK >();
  reg.register_algorithm< detect_features_FAST >();
  reg.register_algorithm< detect_features_GFTT >();
  reg.register_algorithm< detect_features_MSER >();
  reg.register_algorithm< detect_features_ORB >();
  reg.register_algorithm< detect_features_simple_blob >();

  reg.register_algorithm< extract_descriptors_BRISK >();
  reg.register_algorithm< extract_descriptors_ORB >();

  reg.register_algorithm< match_features_bruteforce >();
  reg.register_algorithm< match_features_flannbased >();

  reg.register_algorithm< hough_circle_detector >();
  reg.register_algorithm< detect_motion_3frame_differencing >();
  reg.register_algorithm< detect_motion_mog2 >();

  // Conditional algorithms
  // Source ``KWIVER_OCV_HAS_*`` symbol definitions can be found in the header
  //  files of the algorithms referred to.
#ifdef KWIVER_OCV_HAS_AGAST
  reg.register_algorithm< detect_features_AGAST >();
#endif

#ifdef KWIVER_OCV_HAS_BRIEF
  reg.register_algorithm< extract_descriptors_BRIEF >();
#endif

#ifdef KWIVER_OCV_HAS_DAISY
  reg.register_algorithm< extract_descriptors_DAISY >();
#endif

#ifdef KWIVER_OCV_HAS_FREAK
    reg.register_algorithm< extract_descriptors_FREAK >();
#endif

#ifdef KWIVER_OCV_HAS_LATCH
   reg.register_algorithm< extract_descriptors_LATCH >();
#endif

#ifdef KWIVER_OCV_HAS_LUCID
  reg.register_algorithm< extract_descriptors_LUCID >();
#endif

#ifdef KWIVER_OCV_HAS_MSD
  reg.register_algorithm< detect_features_MSD >();
#endif

#ifdef KWIVER_OCV_HAS_SIFT
  reg.register_algorithm< detect_features_SIFT >();
  reg.register_algorithm< extract_descriptors_SIFT >();
#endif

#ifdef KWIVER_OCV_HAS_STAR
  reg.register_algorithm< detect_features_STAR >();
#endif

#ifdef KWIVER_OCV_HAS_SURF
  reg.register_algorithm< detect_features_SURF >();
  reg.register_algorithm< extract_descriptors_SURF >();
#endif

  reg.register_algorithm< detect_heat_map >();

  reg.register_algorithm< refine_detections_write_to_disk >();
  reg.register_algorithm< split_image >();
  reg.register_algorithm< merge_images >();
  reg.register_algorithm< track_features_klt >();
  reg.register_algorithm< estimate_pnp >();
  reg.register_algorithm< resection_camera >();

  reg.mark_module_as_loaded();
}

} // end namespace ocv
} // end namespace arrows
} // end namespace kwiver
