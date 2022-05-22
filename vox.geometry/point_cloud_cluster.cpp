//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include <unordered_set>

#include "vox.base/logging.h"
#include "vox.base/parallel.h"
#include "vox.base/progress_bar.h"
#include "vox.geometry/kdtree_flann.h"
#include "vox.geometry/point_cloud.h"

namespace vox::geometry {

std::vector<int> PointCloud::ClusterDBSCAN(double eps, size_t min_points, bool print_progress) const {
    KDTreeFlann kdtree(*this);

    // Precompute all neighbors.
    LOGD("Precompute neighbors.")
    utility::ProgressBar progress_bar(points_.size(), "Precompute neighbors.", print_progress);
    std::vector<std::vector<int>> nbs(points_.size());
#pragma omp parallel for schedule(static) num_threads(utility::EstimateMaxThreads()) \
        shared(kdtree, eps, nbs, progress_bar) default(none)
    for (int idx = 0; idx < int(points_.size()); ++idx) {
        std::vector<double> dists2;
        kdtree.SearchRadius(points_[idx], eps, nbs[idx], dists2);

#pragma omp critical(ClusterDBSCAN)
        { ++progress_bar; }
    }
    LOGD("Done Precompute neighbors.")

    // Set all labels to undefined (-2).
    LOGD("Compute Clusters")
    progress_bar.Reset(points_.size(), "Clustering", print_progress);
    std::vector<int> labels(points_.size(), -2);
    int cluster_label = 0;
    for (size_t idx = 0; idx < points_.size(); ++idx) {
        // Label is not undefined.
        if (labels[idx] != -2) {
            continue;
        }

        // Check density.
        if (nbs[idx].size() < min_points) {
            labels[idx] = -1;
            continue;
        }

        std::unordered_set<int> nbs_next(nbs[idx].begin(), nbs[idx].end());
        std::unordered_set<int> nbs_visited;
        nbs_visited.insert(int(idx));

        labels[idx] = cluster_label;
        ++progress_bar;
        while (!nbs_next.empty()) {
            int nb = *nbs_next.begin();
            nbs_next.erase(nbs_next.begin());
            nbs_visited.insert(nb);

            // Noise label.
            if (labels[nb] == -1) {
                labels[nb] = cluster_label;
                ++progress_bar;
            }
            // Not undefined label.
            if (labels[nb] != -2) {
                continue;
            }
            labels[nb] = cluster_label;
            ++progress_bar;

            if (nbs[nb].size() >= min_points) {
                for (int qnb : nbs[nb]) {
                    if (nbs_visited.count(qnb) == 0) {
                        nbs_next.insert(qnb);
                    }
                }
            }
        }

        cluster_label++;
    }

    LOGD("Done Compute Clusters: {:d}", cluster_label)
    return labels;
}

}  // namespace vox::geometry
