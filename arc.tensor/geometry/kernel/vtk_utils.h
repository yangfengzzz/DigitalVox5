//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include <vtkPolyData.h>
#include <vtkSmartPointer.h>

#include "geometry/geometry.h"
#include "geometry/point_cloud.h"
#include "geometry/triangle_mesh.h"

namespace vox {
namespace t {
namespace geometry {
namespace kernel {
namespace vtkutils {

/// Creates a vtkPolyData object from a point cloud or triangle mesh.
vtkSmartPointer<vtkPolyData> CreateVtkPolyDataFromGeometry(const Geometry& geometry);

/// Creates a triangle mesh from a vtkPolyData object.
TriangleMesh CreateTriangleMeshFromVtkPolyData(vtkPolyData* polyData);

}  // namespace vtkutils
}  // namespace kernel
}  // namespace geometry
}  // namespace t
}  // namespace vox
