// WORK IN PROGRESS - DO NOT REVIEW

#include "Voronoi.h"

static ospray::vec3f curVoronoiPoint;

struct pointCmp
{
    bool operator()(const ospray::vec3f& p1, const ospray::vec3f& p2) const
    {
        float v1 = length/*2*/(p1-curVoronoiPoint);
        float v2 = length/*2*/(p2-curVoronoiPoint);
        bool result0 = v1 < v2;
        //bool result1 = ((float)(p1-curVoronoiPoint).length2()) < ((float)(p2-curVoronoiPoint).length2());
        //apparently result0 is not always result1, because extended precision used in registered is different from precision when values are stored in memory
        return result0;
    }
}pointCompare;

Voronoi::Voronoi()
{
}

Voronoi::~Voronoi()
{
}

float Voronoi::triple( const ospray::vec3f& v0, const ospray::vec3f& v1, const ospray::vec3f& v2 )
{
    return
            v0.x * (v1.y * v2.z - v1.z * v2.y) +
            v0.y * (v1.z * v2.x - v1.x * v2.z) +
            v0.z * (v1.x * v2.y - v1.y * v2.x);
}

void Voronoi::getVerticesInsidePlanes(
        const std::vector<ospray::vec3f>& planes,
        std::vector<ospray::vec3f>& verticesOut,
        std::set<int>& planeIndicesOut)
{
    // Based on btGeometryUtil.cpp (Gino van den Bergen / Erwin Coumans)
    verticesOut.resize(0);
    planeIndicesOut.clear();
    const int numPlanes = planes.size();
    int i, j, k, l;
    for (i=0;i<numPlanes;i++)
    {
        const ospray::vec3f& N1 = planes[i];
        for (j=i+1;j<numPlanes;j++)
        {
            const ospray::vec3f& N2 = planes[j];
            ospray::vec3f n1n2 = cross(N1,N2);
            if (length(n1n2) /*2*/ > 0.0001)
            {
                for (k=j+1;k<numPlanes;k++)
                {
                    const ospray::vec3f& N3 = planes[k];
                    ospray::vec3f n2n3 = cross(N2,N3);
                    ospray::vec3f n3n1 = cross(N3,N1);
                    if (length/*2*/(n2n3) > 0.0001 && length/*2*/(n3n1) > 0.0001 )
                    {
                        float quotient = dot(N1,n2n3);
                        if (fabs(quotient) > 0.0001)
                        {
                            ospray::vec3f potentialVertex = (n2n3 * N1[3] + n3n1 * N2[3] + n1n2 * N3[3]) * (-1. / quotient);
                            for (l=0; l<numPlanes; l++)
                            {
                                const ospray::vec3f& NP = planes[l];
                                if ((dot(NP,potentialVertex)+NP[3]) > 0.000001)
                                    break;
                            }
                            if (l == numPlanes)
                            {
                                // vertex (three plane intersection) inside all planes
                                verticesOut.push_back(potentialVertex);
                                planeIndicesOut.insert(i);
                                planeIndicesOut.insert(j);
                                planeIndicesOut.insert(k);
                            }
                        }
                    }
                }
            }
        }
    }
}

void Voronoi::voronoiBBShatter(
        const std::vector<ospray::vec3f>& points,
        const ospray::vec3f& bbmin,
        const ospray::vec3f& bbmax,
        const embree::QuaternionT<float>& bbq,
        const ospray::vec3f& bbt)
{
    // points define voronoi cells in world space (avoid duplicates)
    // bbmin & bbmax = bounding box min and max in local space
    // bbq & bbt = bounding box quaternion rotation and translation
    // matDensity = Material density for voronoi shard mass calculation
    ospray::vec3f bbvx = bbq.rotate(ospray::vec3f(1.0, 0.0, 0.0), 1).r;
    ospray::vec3f bbvy = bbq.rotate(ospray::vec3f(0.0, 1.0, 0.0), 1).i;
    ospray::vec3f bbvz = bbq.rotate(ospray::vec3f(0.0, 0.0, 1.0), 1).j;
    embree::QuaternionT<float> bbiq = -bbq;
    ConvexHullComputer* convexHC = new ConvexHullComputer();
    std::vector<ospray::vec3f> vertices;
    ospray::vec3f rbb, nrbb;
    float nlength, maxDistance, distance;
    std::vector<ospray::vec3f> sortedVoronoiPoints;

    for(unsigned int i = 0; i < points.size(); i++)
        sortedVoronoiPoints.push_back(points[i]);

    ospray::vec3f normal, plane;
    std::vector<ospray::vec3f> planes;
    std::set<int> planeIndices;
    std::set<int>::iterator planeIndicesIter;
    int numplaneIndices;
    int cellnum = 0;
    int i, j, k;

    int numpoints = points.size();
    for (i=0; i < numpoints ;i++) {
        curVoronoiPoint = points[i];
        ospray::vec3f icp = bbiq.rotate(curVoronoiPoint - bbt, 1).r;
        rbb = icp - bbmax;
        nrbb = bbmin - icp;
        planes.resize(6);
        planes[0] = bbvx; planes[0][3] = rbb.x;
        planes[1] = bbvy; planes[1][3] = rbb.y;
        planes[2] = bbvz; planes[2][3] = rbb.z;
        planes[3] = -bbvx; planes[3][3] = nrbb.x;
        planes[4] = -bbvy; planes[4][3] = nrbb.y;
        planes[5] = -bbvz; planes[5][3] = nrbb.z;
        maxDistance = std::numeric_limits<float>::max();
        sort(sortedVoronoiPoints.begin(), sortedVoronoiPoints.end(), pointCompare);
        //sortedVoronoiPoints(pointCmp());
        for (j=1; j < numpoints; j++) {
            normal = sortedVoronoiPoints[j] - curVoronoiPoint;
            nlength = length(normal);
            if (nlength > maxDistance)
                break;
            plane = normalize(normal);
            plane[3] = -nlength / 2.;
            planes.push_back(plane);
            getVerticesInsidePlanes(planes, vertices, planeIndices);
            if (vertices.size() == 0)
                break;
            numplaneIndices = planeIndices.size();
            if (numplaneIndices != planes.size()) {
                planeIndicesIter = planeIndices.begin();
                for (k=0; k < numplaneIndices; k++) {
                    if (k != *planeIndicesIter)
                        planes[k] = planes[*planeIndicesIter];
                    planeIndicesIter++;
                }
                planes.resize(numplaneIndices);
            }
            maxDistance = length(vertices[0]);
            for (k=1; k < vertices.size(); k++) {
                distance = length(vertices[k]);
                if (maxDistance < distance)
                    maxDistance = distance;
            }
            maxDistance *= 2.0;
        }
        if (vertices.size() == 0)
            continue;

        // Clean-up voronoi convex shard vertices and generate edges & faces
        convexHC->compute(&vertices[0].x, sizeof(ospray::vec3f), vertices.size(), CONVEX_MARGIN,0.0);

        // At this point we have a complete 3D voronoi shard mesh contained in convexHC

        // Calculate volume and center of mass (Stan Melax volume integration)
        int numFaces = convexHC->faces.size();
        int v0, v1, v2; // Triangle vertices
        float volume = 0.0;
        ospray::vec3f com(0.0, 0.0, 0.0);
        for (j=0; j < numFaces; j++) {
            const ConvexHullComputer::Edge* edge = &convexHC->edges[convexHC->faces[j]];
            v0 = edge->getSourceVertex();
            v1 = edge->getTargetVertex();
            edge = edge->getNextEdgeOfFace();
            v2 = edge->getTargetVertex();
            while (v2 != v0)
            {
                // Counter-clockwise triangulated voronoi shard mesh faces (v0-v1-v2) and edges here...
                float vol = triple(convexHC->vertices[v0], convexHC->vertices[v1], convexHC->vertices[v2]);
                volume += vol;
                com += vol * (convexHC->vertices[v0] + convexHC->vertices[v1] + convexHC->vertices[v2]);
                edge = edge->getNextEdgeOfFace();
                v1 = v2;
                v2 = edge->getTargetVertex();
            }
        }
        com = com / (volume * 4.0);
        volume /= 6.0;

        // Shift all vertices relative to center of mass
        int numVerts = convexHC->vertices.size();
        for (j=0; j < numVerts; j++)
        {
            convexHC->vertices[j] -= com;
        }
        cellnum ++;
    }
}

void Voronoi::voronoiConvexHullShatter(
        const std::vector<ospray::vec3f>& points,
        const std::vector<ospray::vec3f>& verts,
        const embree::QuaternionT<float>& bbq,
        const ospray::vec3f& bbt)
{
    // points define voronoi cells in world space (avoid duplicates)
    // verts = source (convex hull) mesh vertices in local space
    // bbq & bbt = source (convex hull) mesh quaternion rotation and translation
    // matDensity = Material density for voronoi shard mass calculation
    ConvexHullComputer* convexHC = new ConvexHullComputer();
    std::vector<ospray::vec3f> vertices, chverts;
    ospray::vec3f rbb, nrbb;
    float nlength, maxDistance, distance;
    std::vector<ospray::vec3f> sortedVoronoiPoints;

    for(unsigned int i = 0; i < points.size(); i++)
    {
        sortedVoronoiPoints.push_back(points[i]);
    }

    ospray::vec3f normal, plane;
    std::vector<ospray::vec3f> planes, convexPlanes;
    std::set<int> planeIndices;
    std::set<int>::iterator planeIndicesIter;
    int numplaneIndices;
    int cellnum = 0;
    int i, j, k;

    // Convert verts to world space and get convexPlanes
    int numverts = verts.size();
    chverts.resize(verts.size());
    for (i=0; i < numverts ;i++)
    {
        chverts[i] = bbq.rotate(verts[i] + bbt,1).r;
    }
    //btGeometryUtil::getPlaneEquationsFromVertices(chverts, convexPlanes);
    // Using convexHullComputer faster than getPlaneEquationsFromVertices for large meshes...
    convexHC->compute( &chverts[0].x, sizeof(ospray::vec3f), numverts, 0.0, 0.0 );
    int numFaces = convexHC->faces.size();
    int v0, v1, v2; // vertices
    for (i=0; i < numFaces; i++)
    {
        const ConvexHullComputer::Edge* edge = &convexHC->edges[convexHC->faces[i]];
        v0 = edge->getSourceVertex();
        v1 = edge->getTargetVertex();
        edge = edge->getNextEdgeOfFace();
        v2 = edge->getTargetVertex();
        plane = normalize(
                    cross(convexHC->vertices[v1]-convexHC->vertices[v0],
                          convexHC->vertices[v2]-convexHC->vertices[v0]));
        plane[3] = -dot(plane, convexHC->vertices[v0]);
        convexPlanes.push_back(plane);
    }
    const int numconvexPlanes = convexPlanes.size();

    int numpoints = points.size();
    for (i=0; i < numpoints ;i++) {
        curVoronoiPoint = points[i];
        planes.clear();
        for(unsigned int i = 0; i < convexPlanes.size(); i++)
            planes.push_back(convexPlanes[i]);

        for (j=0; j < numconvexPlanes ;j++)
        {
            planes[j][3] += dot(planes[j], curVoronoiPoint);
        }

        maxDistance = std::numeric_limits<float>::max();
        sort(sortedVoronoiPoints.begin(), sortedVoronoiPoints.end(), pointCompare);
        //sortedVoronoiPoints.heapSort(pointCmp());
        for (j=1; j < numpoints; j++) {
            normal = sortedVoronoiPoints[j] - curVoronoiPoint;
            nlength = length(normal);
            if (nlength > maxDistance)
                break;
            plane = normalize(normal);
            plane[3] = -nlength / float(2.);
            planes.push_back(plane);
            this->getVerticesInsidePlanes(planes, vertices, planeIndices);
            if (vertices.size() == 0)
                break;
            numplaneIndices = planeIndices.size();
            if (numplaneIndices != planes.size())
            {
                planeIndicesIter = planeIndices.begin();
                for (k=0; k < numplaneIndices; k++)
                {
                    if (k != *planeIndicesIter)
                        planes[k] = planes[*planeIndicesIter];
                    planeIndicesIter++;
                }
                planes.resize(numplaneIndices);
            }
            maxDistance = length(vertices[0]);
            for (k=1; k < vertices.size(); k++)
            {
                distance = length(vertices[k]);
                if (maxDistance < distance)
                {  maxDistance = distance;
                }
                maxDistance *= float(2.);
            }
            if (vertices.size() == 0)
                continue;

            // Clean-up voronoi convex shard vertices and generate edges & faces
            convexHC->compute(&vertices[0].x, sizeof(ospray::vec3f), vertices.size(),0.0,0.0);

            // At this point we have a complete 3D voronoi shard mesh contained in convexHC

            // Calculate volume and center of mass (Stan Melax volume integration)
            numFaces = convexHC->faces.size();
            float volume = float(0.);
            ospray::vec3f com(0., 0., 0.);
            for (j=0; j < numFaces; j++)
            {
                const ConvexHullComputer::Edge* edge = &convexHC->edges[convexHC->faces[j]];
                v0 = edge->getSourceVertex();
                v1 = edge->getTargetVertex();
                edge = edge->getNextEdgeOfFace();
                v2 = edge->getTargetVertex();
                while (v2 != v0)
                {
                    // Counter-clockwise triangulated voronoi shard mesh faces (v0-v1-v2) and edges here...
                    float vol = triple( convexHC->vertices[v0], convexHC->vertices[v1], convexHC->vertices[v2]);
                    volume += vol;
                    com += vol * (convexHC->vertices[v0] + convexHC->vertices[v1] + convexHC->vertices[v2]);
                    edge = edge->getNextEdgeOfFace();
                    v1 = v2;
                    v2 = edge->getTargetVertex();
                }
            }
            com /= volume * float(4.);
            volume /= float(6.);

            // Shift all vertices relative to center of mass
            int numVerts = convexHC->vertices.size();
            for (j=0; j < numVerts; j++)
            {
                convexHC->vertices[j] -= com;
            }

            cellnum ++;
        }
    }
}


float ConvexHullComputer::compute(const void* coords, bool doubleCoords, int stride, int count, float shrink, float shrinkClamp)
{
    if (count <= 0)
    {
        vertices.clear();
        edges.clear();
        faces.clear();
        return 0;
    }
    ConvexHullInternal hull;
    hull.compute(coords, doubleCoords, stride, count);
    float shift = 0;
    if ((shrink > 0) && ((shift = hull.shrink(shrink, shrinkClamp)) < 0))
    {
        vertices.clear();
        edges.clear();
        faces.clear();
        return shift;
    }
    vertices.resize(0);
    edges.resize(0);
    faces.resize(0);

    std::vector<ConvexHullInternal::Vertex*> oldVertices;
    getVertexCopy(hull.vertexList, oldVertices);
    int copied = 0;
    while (copied < oldVertices.size())
    {
        btConvexHullInternal::Vertex* v = oldVertices[copied];
        vertices.push_back(hull.getCoordinates(v));
        btConvexHullInternal::Edge* firstEdge = v->edges;
        if (firstEdge)
        {
            int firstCopy = -1;
            int prevCopy = -1;
            btConvexHullInternal::Edge* e = firstEdge;
            do
            {
                if (e->copy < 0)
                {
                    int s = edges.size();
                    edges.push_back(Edge());
                    edges.push_back(Edge());
                    Edge* c = &edges[s];
                    Edge* r = &edges[s + 1];
                    e->copy = s;
                    e->reverse->copy = s + 1;
                    c->reverse = 1;
                    r->reverse = -1;
                    c->targetVertex = getVertexCopy(e->target, oldVertices);
                    r->targetVertex = copied;
                }
                if (prevCopy >= 0)
                {
                    edges[e->copy].next = prevCopy - e->copy;
                }
                else
                {
                    firstCopy = e->copy;
                }
                prevCopy = e->copy;
                e = e->next;
            } while (e != firstEdge);
            edges[firstCopy].next = prevCopy - firstCopy;
        }
        copied++;
    }
    for (int i = 0; i < copied; i++)
    {
        ConvexHullInternal::Vertex* v = oldVertices[i];
        ConvexHullInternal::Edge* firstEdge = v->edges;
        if (firstEdge)
        {
            ConvexHullInternal::Edge* e = firstEdge;
            do
            {
                if (e->copy >= 0)
                {
                    faces.push_back(e->copy);
                    btConvexHullInternal::Edge* f = e;
                    do
                    {
                        f->copy = -1;
                        f = f->reverse->prev;
                    } while (f != e);
                }
                e = e->next;
            }
            while (e != firstEdge);
        }
    }
    return shift;
}
