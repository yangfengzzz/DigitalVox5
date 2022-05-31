//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <vector>

#include "vox.force/common.h"

namespace vox::force {
/** This class encapsulates the state of all vertices.
 * All parameters are stored in individual arrays.
 */
class VertexData {
private:
    std::vector<Vector3r> m_x_;

public:
    FORCE_INLINE VertexData() : m_x_() {}

    FORCE_INLINE ~VertexData() { m_x_.clear(); }

    FORCE_INLINE void AddVertex(const Vector3r &vertex) { m_x_.push_back(vertex); }

    FORCE_INLINE Vector3r &GetPosition(const unsigned int i) { return m_x_[i]; }

    [[nodiscard]] FORCE_INLINE const Vector3r &GetPosition(const unsigned int i) const { return m_x_[i]; }

    FORCE_INLINE void SetPosition(const unsigned int i, const Vector3r &pos) { m_x_[i] = pos; }

    /** Resize the array containing the particle data.
     */
    FORCE_INLINE void Resize(const unsigned int new_size) { m_x_.resize(new_size); }

    /** Reserve the array containing the particle data.
     */
    FORCE_INLINE void Reserve(const unsigned int new_size) { m_x_.reserve(new_size); }

    /** Release the array containing the particle data.
     */
    FORCE_INLINE void Release() { m_x_.clear(); }

    /** Release the array containing the particle data.
     */
    [[nodiscard]] FORCE_INLINE unsigned int Size() const { return (unsigned int)m_x_.size(); }

    [[nodiscard]] FORCE_INLINE const std::vector<Vector3r> &GetVertices() const { return m_x_; }
};

/** This class encapsulates the state of all particles of a particle model.
 * All parameters are stored in individual arrays.
 */
class ParticleData {
private:
    // Mass
    // If the mass is zero, the particle is static
    std::vector<Real> m_masses_;
    std::vector<Real> m_inv_masses_;

    // Dynamic state
    std::vector<Vector3r> m_x_0_;
    std::vector<Vector3r> m_x_;
    std::vector<Vector3r> m_v_;
    std::vector<Vector3r> m_a_;
    std::vector<Vector3r> m_old_x_;
    std::vector<Vector3r> m_last_x_;

public:
    FORCE_INLINE ParticleData()
        : m_masses_(), m_inv_masses_(), m_x_0_(), m_x_(), m_v_(), m_a_(), m_old_x_(), m_last_x_() {}

    FORCE_INLINE ~ParticleData() {
        m_masses_.clear();
        m_inv_masses_.clear();
        m_x_0_.clear();
        m_x_.clear();
        m_v_.clear();
        m_a_.clear();
        m_old_x_.clear();
        m_last_x_.clear();
    }

    FORCE_INLINE void AddVertex(const Vector3r &vertex) {
        m_x_0_.push_back(vertex);
        m_x_.push_back(vertex);
        m_old_x_.push_back(vertex);
        m_last_x_.push_back(vertex);
        m_masses_.push_back(1.0);
        m_inv_masses_.push_back(1.0);
        m_v_.emplace_back(0.0, 0.0, 0.0);
        m_a_.emplace_back(0.0, 0.0, 0.0);
    }

    FORCE_INLINE Vector3r &GetPosition(const unsigned int i) { return m_x_[i]; }

    [[nodiscard]] FORCE_INLINE const Vector3r &GetPosition(const unsigned int i) const { return m_x_[i]; }

    FORCE_INLINE void SetPosition(const unsigned int i, const Vector3r &pos) { m_x_[i] = pos; }

    FORCE_INLINE Vector3r &GetPosition0(const unsigned int i) { return m_x_0_[i]; }

    [[nodiscard]] FORCE_INLINE const Vector3r &GetPosition0(const unsigned int i) const { return m_x_0_[i]; }

    FORCE_INLINE void SetPosition0(const unsigned int i, const Vector3r &pos) { m_x_0_[i] = pos; }

    FORCE_INLINE Vector3r &GetLastPosition(const unsigned int i) { return m_last_x_[i]; }

    [[nodiscard]] FORCE_INLINE const Vector3r &GetLastPosition(const unsigned int i) const { return m_last_x_[i]; }

    FORCE_INLINE void SetLastPosition(const unsigned int i, const Vector3r &pos) { m_last_x_[i] = pos; }

    FORCE_INLINE Vector3r &GetOldPosition(const unsigned int i) { return m_old_x_[i]; }

    [[nodiscard]] FORCE_INLINE const Vector3r &GetOldPosition(const unsigned int i) const { return m_old_x_[i]; }

    FORCE_INLINE void SetOldPosition(const unsigned int i, const Vector3r &pos) { m_old_x_[i] = pos; }

    FORCE_INLINE Vector3r &GetVelocity(const unsigned int i) { return m_v_[i]; }

    [[nodiscard]] FORCE_INLINE const Vector3r &GetVelocity(const unsigned int i) const { return m_v_[i]; }

    FORCE_INLINE void SetVelocity(const unsigned int i, const Vector3r &vel) { m_v_[i] = vel; }

    FORCE_INLINE Vector3r &GetAcceleration(const unsigned int i) { return m_a_[i]; }

    [[nodiscard]] FORCE_INLINE const Vector3r &GetAcceleration(const unsigned int i) const { return m_a_[i]; }

    FORCE_INLINE void SetAcceleration(const unsigned int i, const Vector3r &accel) { m_a_[i] = accel; }

    [[nodiscard]] FORCE_INLINE Real GetMass(const unsigned int i) const { return m_masses_[i]; }

    FORCE_INLINE Real &GetMass(const unsigned int i) { return m_masses_[i]; }

    FORCE_INLINE void SetMass(const unsigned int i, const Real mass) {
        m_masses_[i] = mass;
        if (mass != 0.0)
            m_inv_masses_[i] = static_cast<Real>(1.0) / mass;
        else
            m_inv_masses_[i] = 0.0;
    }

    [[nodiscard]] FORCE_INLINE Real GetInvMass(const unsigned int i) const { return m_inv_masses_[i]; }

    [[nodiscard]] FORCE_INLINE unsigned int GetNumberOfParticles() const { return (unsigned int)m_x_.size(); }

    [[nodiscard]] FORCE_INLINE const std::vector<Vector3r> &GetVertices() const { return m_x_; }

    /** Resize the array containing the particle data.
     */
    FORCE_INLINE void Resize(const unsigned int new_size) {
        m_masses_.resize(new_size);
        m_inv_masses_.resize(new_size);
        m_x_0_.resize(new_size);
        m_x_.resize(new_size);
        m_v_.resize(new_size);
        m_a_.resize(new_size);
        m_old_x_.resize(new_size);
        m_last_x_.resize(new_size);
    }

    /** Reserve the array containing the particle data.
     */
    FORCE_INLINE void Reserve(const unsigned int new_size) {
        m_masses_.reserve(new_size);
        m_inv_masses_.reserve(new_size);
        m_x_0_.reserve(new_size);
        m_x_.reserve(new_size);
        m_v_.reserve(new_size);
        m_a_.reserve(new_size);
        m_old_x_.reserve(new_size);
        m_last_x_.reserve(new_size);
    }

    /** Release the array containing the particle data.
     */
    FORCE_INLINE void Release() {
        m_masses_.clear();
        m_inv_masses_.clear();
        m_x_0_.clear();
        m_x_.clear();
        m_v_.clear();
        m_a_.clear();
        m_old_x_.clear();
        m_last_x_.clear();
    }

    /** Release the array containing the particle data.
     */
    [[nodiscard]] FORCE_INLINE unsigned int Size() const { return (unsigned int)m_x_.size(); }
};

/** This class encapsulates the state of all orientations of a quaternion model.
 * All parameters are stored in individual arrays.
 */
class OrientationData {
private:
    // Mass
    // If the mass is zero, the particle is static
    std::vector<Real> m_masses_;
    std::vector<Real> m_inv_masses_;

    // Dynamic state
    std::vector<Quaternionr> m_q_0_;
    std::vector<Quaternionr> m_q_;
    std::vector<Vector3r> m_omega_;
    std::vector<Vector3r> m_alpha_;
    std::vector<Quaternionr> m_old_q_;
    std::vector<Quaternionr> m_last_q_;

public:
    FORCE_INLINE OrientationData()
        : m_masses_(), m_inv_masses_(), m_q_0_(), m_q_(), m_omega_(), m_alpha_(), m_old_q_(), m_last_q_() {}

    FORCE_INLINE ~OrientationData() {
        m_masses_.clear();
        m_inv_masses_.clear();
        m_q_0_.clear();
        m_q_.clear();
        m_omega_.clear();
        m_alpha_.clear();
        m_old_q_.clear();
        m_last_q_.clear();
    }

    FORCE_INLINE void AddQuaternion(const Quaternionr &vertex) {
        m_q_0_.push_back(vertex);
        m_q_.push_back(vertex);
        m_old_q_.push_back(vertex);
        m_last_q_.push_back(vertex);
        m_masses_.push_back(1.0);
        m_inv_masses_.push_back(1.0);
        m_omega_.emplace_back(0.0, 0.0, 0.0);
        m_alpha_.emplace_back(0.0, 0.0, 0.0);
    }

    FORCE_INLINE Quaternionr &GetQuaternion(const unsigned int i) { return m_q_[i]; }

    [[nodiscard]] FORCE_INLINE const Quaternionr &GetQuaternion(const unsigned int i) const { return m_q_[i]; }

    FORCE_INLINE void SetQuaternion(const unsigned int i, const Quaternionr &pos) { m_q_[i] = pos; }

    FORCE_INLINE Quaternionr &GetQuaternion0(const unsigned int i) { return m_q_0_[i]; }

    [[nodiscard]] FORCE_INLINE const Quaternionr &GetQuaternion0(const unsigned int i) const { return m_q_0_[i]; }

    FORCE_INLINE void SetQuaternion0(const unsigned int i, const Quaternionr &pos) { m_q_0_[i] = pos; }

    FORCE_INLINE Quaternionr &GetLastQuaternion(const unsigned int i) { return m_last_q_[i]; }

    [[nodiscard]] FORCE_INLINE const Quaternionr &GetLastQuaternion(const unsigned int i) const { return m_last_q_[i]; }

    FORCE_INLINE void SetLastQuaternion(const unsigned int i, const Quaternionr &pos) { m_last_q_[i] = pos; }

    FORCE_INLINE Quaternionr &GetOldQuaternion(const unsigned int i) { return m_old_q_[i]; }

    [[nodiscard]] FORCE_INLINE const Quaternionr &GetOldQuaternion(const unsigned int i) const { return m_old_q_[i]; }

    FORCE_INLINE void SetOldQuaternion(const unsigned int i, const Quaternionr &pos) { m_old_q_[i] = pos; }

    FORCE_INLINE Vector3r &GetVelocity(const unsigned int i) { return m_omega_[i]; }

    [[nodiscard]] FORCE_INLINE const Vector3r &GetVelocity(const unsigned int i) const { return m_omega_[i]; }

    FORCE_INLINE void SetVelocity(const unsigned int i, const Vector3r &vel) { m_omega_[i] = vel; }

    FORCE_INLINE Vector3r &GetAcceleration(const unsigned int i) { return m_alpha_[i]; }

    [[nodiscard]] FORCE_INLINE const Vector3r &GetAcceleration(const unsigned int i) const { return m_alpha_[i]; }

    FORCE_INLINE void SetAcceleration(const unsigned int i, const Vector3r &accel) { m_alpha_[i] = accel; }

    [[nodiscard]] FORCE_INLINE Real GetMass(const unsigned int i) const { return m_masses_[i]; }

    FORCE_INLINE Real &GetMass(const unsigned int i) { return m_masses_[i]; }

    FORCE_INLINE void SetMass(const unsigned int i, const Real mass) {
        m_masses_[i] = mass;
        if (mass != 0.0)
            m_inv_masses_[i] = static_cast<Real>(1.0) / mass;
        else
            m_inv_masses_[i] = 0.0;
    }

    [[nodiscard]] FORCE_INLINE Real GetInvMass(const unsigned int i) const { return m_inv_masses_[i]; }

    [[nodiscard]] FORCE_INLINE unsigned int GetNumberOfQuaternions() const { return (unsigned int)m_q_.size(); }

    /** Resize the array containing the particle data.
     */
    FORCE_INLINE void Resize(const unsigned int new_size) {
        m_masses_.resize(new_size);
        m_inv_masses_.resize(new_size);
        m_q_0_.resize(new_size);
        m_q_.resize(new_size);
        m_omega_.resize(new_size);
        m_alpha_.resize(new_size);
        m_old_q_.resize(new_size);
        m_last_q_.resize(new_size);
    }

    /** Reserve the array containing the particle data.
     */
    FORCE_INLINE void Reserve(const unsigned int new_size) {
        m_masses_.reserve(new_size);
        m_inv_masses_.reserve(new_size);
        m_q_0_.reserve(new_size);
        m_q_.reserve(new_size);
        m_omega_.reserve(new_size);
        m_alpha_.reserve(new_size);
        m_old_q_.reserve(new_size);
        m_last_q_.reserve(new_size);
    }

    /** Release the array containing the particle data.
     */
    FORCE_INLINE void Release() {
        m_masses_.clear();
        m_inv_masses_.clear();
        m_q_0_.clear();
        m_q_.clear();
        m_omega_.clear();
        m_alpha_.clear();
        m_old_q_.clear();
        m_last_q_.clear();
    }

    /** Release the array containing the particle data.
     */
    [[nodiscard]] FORCE_INLINE unsigned int Size() const { return (unsigned int)m_q_.size(); }
};
}  // namespace vox::force
