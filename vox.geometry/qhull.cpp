//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.geometry/qhull.h"

#include <libqhullcpp/PointCoordinates.h>
#include <libqhullcpp/Qhull.h>
#include <libqhullcpp/QhullFacet.h>
#include <libqhullcpp/QhullFacetList.h>
#include <libqhullcpp/QhullVertexSet.h>

#include "vox.base/logging.h"
#include "vox.geometry/tetra_mesh.h"
#include "vox.geometry/triangle_mesh.h"

namespace vox::geometry {

std::tuple<std::shared_ptr<TriangleMesh>, std::vector<size_t>> Qhull::ComputeConvexHull(
        const std::vector<Eigen::Vector3d>& points, bool joggle_inputs) {
    auto convex_hull = std::make_shared<TriangleMesh>();
    std::vector<size_t> pt_map;

    std::vector<double> qhull_points_data(points.size() * 3);
    for (size_t pidx = 0; pidx < points.size(); ++pidx) {
        const auto& pt = points[pidx];
        qhull_points_data[pidx * 3 + 0] = pt(0);
        qhull_points_data[pidx * 3 + 1] = pt(1);
        qhull_points_data[pidx * 3 + 2] = pt(2);
    }

    orgQhull::PointCoordinates qhull_points(3, "");
    qhull_points.append(qhull_points_data);

    orgQhull::Qhull qhull;
    std::string options = "Qt";
    if (joggle_inputs) {
        options += " QJ";
    }
    qhull.runQhull(qhull_points.comment().c_str(), qhull_points.dimension(), qhull_points.count(),
                   qhull_points.coordinates(), options.c_str());

    orgQhull::QhullFacetList facets = qhull.facetList();
    convex_hull->triangles_.resize(facets.count());
    std::unordered_map<int, int> vert_map;
    std::unordered_set<int> inserted_vertices;
    int tidx = 0;
    for (orgQhull::QhullFacetList::iterator it = facets.begin(); it != facets.end(); ++it) {
        if (!(*it).isGood()) continue;

        orgQhull::QhullFacet f = *it;
        orgQhull::QhullVertexSet vSet = f.vertices();
        int triangle_subscript = 0;
        for (auto&& vIt : vSet) {
            orgQhull::QhullVertex v = vIt;
            orgQhull::QhullPoint p = v.point();

            int vidx = p.id();
            convex_hull->triangles_[tidx](triangle_subscript) = vidx;
            triangle_subscript++;

            if (inserted_vertices.count(vidx) == 0) {
                inserted_vertices.insert(vidx);
                vert_map[vidx] = int(convex_hull->vertices_.size());
                double* coords = p.coordinates();
                convex_hull->vertices_.emplace_back(coords[0], coords[1], coords[2]);
                pt_map.push_back(vidx);
            }
        }

        tidx++;
    }

    auto center = convex_hull->GetCenter();
    for (Eigen::Vector3i& triangle : convex_hull->triangles_) {
        triangle(0) = vert_map[triangle(0)];
        triangle(1) = vert_map[triangle(1)];
        triangle(2) = vert_map[triangle(2)];

        Eigen::Vector3d e1 = convex_hull->vertices_[triangle(1)] - convex_hull->vertices_[triangle(0)];
        Eigen::Vector3d e2 = convex_hull->vertices_[triangle(2)] - convex_hull->vertices_[triangle(0)];
        auto normal = e1.cross(e2);

        auto triangle_center = (1. / 3) * (convex_hull->vertices_[triangle(0)] + convex_hull->vertices_[triangle(1)] +
                                           convex_hull->vertices_[triangle(2)]);
        if (normal.dot(triangle_center - center) < 0) {
            std::swap(triangle(0), triangle(1));
        }
    }

    return std::make_tuple(convex_hull, pt_map);
}

std::tuple<std::shared_ptr<TetraMesh>, std::vector<size_t>> Qhull::ComputeDelaunayTetrahedralization(
        const std::vector<Eigen::Vector3d>& points) {
    typedef decltype(TetraMesh::tetras_)::value_type Vector4i;
    auto delaunay_triangulation = std::make_shared<TetraMesh>();
    std::vector<size_t> pt_map;

    if (points.size() < 4) {
        LOGE("Not enough points to create a tetrahedral mesh.")
    }

    // qhull cannot deal with this case
    if (points.size() == 4) {
        delaunay_triangulation->vertices_ = points;
        delaunay_triangulation->tetras_.emplace_back(0, 1, 2, 3);
        pt_map.insert(pt_map.end(), {0, 1, 2, 3});
        return std::make_tuple(delaunay_triangulation, pt_map);
    }

    std::vector<double> qhull_points_data(points.size() * 3);
    for (size_t pidx = 0; pidx < points.size(); ++pidx) {
        const auto& pt = points[pidx];
        qhull_points_data[pidx * 3 + 0] = pt(0);
        qhull_points_data[pidx * 3 + 1] = pt(1);
        qhull_points_data[pidx * 3 + 2] = pt(2);
    }

    orgQhull::PointCoordinates qhull_points(3, "");
    qhull_points.append(qhull_points_data);

    orgQhull::Qhull qhull;
    qhull.runQhull(qhull_points.comment().c_str(), qhull_points.dimension(), qhull_points.count(),
                   qhull_points.coordinates(), "d Qbb Qt");

    orgQhull::QhullFacetList facets = qhull.facetList();
    delaunay_triangulation->tetras_.resize(facets.count());
    std::unordered_map<int, int> vert_map;
    std::unordered_set<int> inserted_vertices;
    int tidx = 0;
    for (orgQhull::QhullFacetList::iterator it = facets.begin(); it != facets.end(); ++it) {
        if (!(*it).isGood()) continue;

        orgQhull::QhullFacet f = *it;
        orgQhull::QhullVertexSet vSet = f.vertices();
        int tetra_subscript = 0;
        for (auto&& vIt : vSet) {
            orgQhull::QhullVertex v = vIt;
            orgQhull::QhullPoint p = v.point();

            int vidx = p.id();
            delaunay_triangulation->tetras_[tidx](tetra_subscript) = vidx;
            tetra_subscript++;

            if (inserted_vertices.count(vidx) == 0) {
                inserted_vertices.insert(vidx);
                vert_map[vidx] = int(delaunay_triangulation->vertices_.size());
                double* coords = p.coordinates();
                delaunay_triangulation->vertices_.emplace_back(coords[0], coords[1], coords[2]);
                pt_map.push_back(vidx);
            }
        }

        tidx++;
    }

    for (auto& tetra : delaunay_triangulation->tetras_) {
        tetra(0) = vert_map[tetra(0)];
        tetra(1) = vert_map[tetra(1)];
        tetra(2) = vert_map[tetra(2)];
        tetra(3) = vert_map[tetra(3)];
    }

    return std::make_tuple(delaunay_triangulation, pt_map);
}

}  // namespace vox::geometry
