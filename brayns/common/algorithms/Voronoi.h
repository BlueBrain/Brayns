// WORK IN PROGRESS - DO NOT REVIEW

#ifndef _VORONOI_H
#define _VORONOI_H
//#include "ospray::vec3f.h"
//#include "btQuaternion.h"
//#include "btConvexHullComputer.h"

#include "types.h"

#include <vector>
#include <set>
#include <algorithm>

#define CONVEX_MARGIN 0.04

class Voronoi
{
public:
    Voronoi();
    ~Voronoi();

    void getVerticesInsidePlanes(
        const std::vector<ospray::vec3f>& planes,
        std::vector<ospray::vec3f>& verticesOut,
        std::set<int>& planeIndicesOut);

    void voronoiBBShatter(
        const std::vector<ospray::vec3f>& points,
        const ospray::vec3f& bbmin,
        const ospray::vec3f& bbmax,
        const embree::QuaternionT<float>& bbq,
        const ospray::vec3f& bbt);

    void voronoiConvexHullShatter(
        const std::vector<ospray::vec3f>& points,
        const std::vector<ospray::vec3f>& verts,
        const embree::QuaternionT<float>& bbq,
        const ospray::vec3f& bbt);

private:
    float triple( const ospray::vec3f& v0, const ospray::vec3f& v1, const ospray::vec3f& v2 );

};

/// Convex hull implementation based on Preparata and Hong
/// See http://code.google.com/p/bullet/issues/detail?id=275
/// Ole Kniemeyer, MAXON Computer GmbH
class ConvexHullComputer
{
private:
    float compute(const void* coords, bool doubleCoords, int stride, int count, float shrink, float shrinkClamp);

public:

    class Edge
    {
    private:
        int next;
        int reverse;
        int targetVertex;

        friend class ConvexHullComputer;

    public:
        int getSourceVertex() const
        {
            return (this + reverse)->targetVertex;
        }

        int getTargetVertex() const
        {
            return targetVertex;
        }

        const Edge* getNextEdgeOfVertex() const // clockwise list of all edges of a vertex
        {
            return this + next;
        }

        const Edge* getNextEdgeOfFace() const // counter-clockwise list of all edges of a face
        {
            return (this + reverse)->getNextEdgeOfVertex();
        }

        const Edge* getReverseEdge() const
        {
            return this + reverse;
        }
    };


    // Vertices of the output hull
    std::vector<ospray::vec3f> vertices;

    // Edges of the output hull
    std::vector<Edge> edges;

    // Faces of the convex hull. Each entry is an index into the "edges" array pointing to an edge of the face. Faces are planar n-gons
    std::vector<int> faces;

    /*
        Compute convex hull of "count" vertices stored in "coords". "stride" is the difference in bytes
        between the addresses of consecutive vertices. If "shrink" is positive, the convex hull is shrunken
        by that amount (each face is moved by "shrink" length units towards the center along its normal).
        If "shrinkClamp" is positive, "shrink" is clamped to not exceed "shrinkClamp * innerRadius", where "innerRadius"
        is the minimum distance of a face to the center of the convex hull.

        The returned value is the amount by which the hull has been shrunken. If it is negative, the amount was so large
        that the resulting convex hull is empty.

        The output convex hull can be found in the member variables "vertices", "edges", "faces".
        */
    float compute(const float* coords, int stride, int count, float shrink, float shrinkClamp)
    {
        return compute(coords, false, stride, count, shrink, shrinkClamp);
    }

    // same as above, but double precision
    float compute(const double* coords, int stride, int count, float shrink, float shrinkClamp)
    {
        return compute(coords, true, stride, count, shrink, shrinkClamp);
    }
};


class ConvexHullInternal
{
public:

    void compute(const void* coords, bool doubleCoords, int stride, int count);
};

#endif
