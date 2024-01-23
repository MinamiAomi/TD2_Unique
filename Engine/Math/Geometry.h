#pragma once

#include <vector>

#include "MathUtils.h"

namespace Math {

    struct Sphere;
    struct AABB;
    struct OBB;
    struct Line;
    struct Ray;
    struct Segment;
    struct Capsule;
    struct Plane;
    struct Triangle;


    struct Sphere {
        Vector3 center;
        float radius = 0.5f;
    };

    struct AABB {
        AABB() = default;
        AABB(const Vector3& min, const Vector3& max) : min(min), max(max) {}
        explicit AABB(const Vector3& minmax) : min(minmax), max(minmax) {}
        explicit AABB(const std::vector<Vector3>& points) : min(points[0]), max(points[0]) {
            for (size_t i = 1; i < points.size(); ++i) {
                Merge(points[i]);
            }
        }

        void Merge(const AABB& other) {
            min = Vector3::Min(min, other.min);
            max = Vector3::Max(max, other.max);
        }
        void Merge(const Vector3& point) {
            min = Vector3::Min(min, point);
            max = Vector3::Max(max, point);
        }

        Vector3 Extent() const { return max - min; }
        float Extent(size_t dim) const { return max[dim] - min[dim]; }

        Vector3 Center() const { return (max + min) * 0.5f; }
        float Center(size_t dim) const { return (max[dim] + min[dim]) * 0.5f; }

        bool Contains(const AABB& other) const {
            return
                min.x <= other.min.x &&
                other.max.x <= max.x &&
                min.y <= other.min.y &&
                other.max.y <= max.y &&
                min.z <= other.min.z &&
                other.max.z <= max.z;
        }
        bool Contains(const Vector3& point) const {
            return
                min.x <= point.x &&
                point.x <= max.x &&
                min.y <= point.y &&
                point.y <= max.y &&
                min.z <= point.z &&
                point.z <= max.z;
        }

        bool Intersect(const AABB& other) const {
            return
                (this->min.x <= other.max.x && this->max.x >= other.min.x) &&
                (this->min.y <= other.max.y && this->max.y >= other.min.y) &&
                (this->min.z <= other.max.z && this->max.z >= other.min.z);
        }

        Vector3 min;
        Vector3 max;
    };

    struct OBB {
        Vector3 center;
        Vector3 orientations[3] = { Vector3::unitX, Vector3::unitY, Vector3::unitZ };
        Vector3 size = Vector3::one;
    };

    struct Line {
        Vector3 origin;
        Vector3 diff;
    };

    struct Ray {
        Vector3 origin;
        Vector3 diff;
    };

    struct Segment {
        Vector3 origin;
        Vector3 diff;
    };

    struct Capsule {
        Segment segment;
        float radius;
    };

    struct Triangle {
        Triangle() = default;
        Triangle(const Vector3& v0, const Vector3& v1, const Vector3& v2) {
            vertices[0] = v0;
            vertices[1] = v1;
            vertices[2] = v2;
        }

        Vector3 Normal() const {
            return Cross(vertices[1] - vertices[0], vertices[2] - vertices[1]).Normalized();
        }

        Vector3 vertices[3];
    };

    struct Plane {
        Plane() = default;
        Plane(const Vector3& normal, float distance) :
            normal(normal),
            distance(distance) {
        }
        Plane(const Vector3& normal, const Vector3& point) :
            normal(normal.Normalized()),
            distance(0.0f) {
            distance = Dot(normal, point);
        }
        explicit Plane(const Triangle& triangle) :
            Plane(triangle.Normal(), triangle.vertices[0]) {
        }


        Vector3 normal;
        float distance;
    };

   
    bool IsCollision(const Sphere& sphere1, const Sphere& sphere2);
    bool IsCollision(const Sphere& sphere, const AABB& aabb);
    bool IsCollision(const Sphere& sphere, const OBB& obb);
    bool IsCollision(const OBB& obb1, const OBB& obb2);
}