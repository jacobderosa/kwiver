// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

/// \file
/// \brief Plugin algorithm registration for MVG Arrow

#include <kwiver_algo_mvg_plugin_export.h>

#include <vital/algo/algorithm_factory.h>

#include <algo/hierarchical_bundle_adjust.h>
#include <algo/integrate_depth_maps.h>
#include <algo/initialize_cameras_landmarks.h>
#include <algo/initialize_cameras_landmarks_basic.h>
#include <algo/triangulate_landmarks.h>

namespace kwiver {
namespace arrows {
namespace mvg {

// ----------------------------------------------------------------------------
extern "C"
KWIVER_ALGO_MVG_PLUGIN_EXPORT
void
register_factories( kwiver::vital::plugin_loader& vpm )
{
  kwiver::vital::algorithm_registrar reg( vpm, "arrows_mvg" );

  if (reg.is_module_loaded())
  {
    return;
  }

  reg.register_algorithm< hierarchical_bundle_adjust >();
  reg.register_algorithm< integrate_depth_maps >();
  reg.register_algorithm< initialize_cameras_landmarks >();
  reg.register_algorithm< initialize_cameras_landmarks_basic >();
  reg.register_algorithm< triangulate_landmarks >();

  reg.mark_module_as_loaded();
}

} // end namespace mvg
} // end namespace arrows
} // end namespace kwiver
