// ----------------------------------------------------------------------------
// -                        Open3D: www.open3d.org                            -
// ----------------------------------------------------------------------------
// The MIT License (MIT)
//
// Copyright (c) 2018-2021 www.open3d.org
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
// ----------------------------------------------------------------------------

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
