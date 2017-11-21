// Copyright (C) Geom Software e.U, Bernhard Kornberger, Graz/Austria
//
// This file is part of the Fade2D library. The student license is free
// of charge and covers personal non-commercial research. Licensees
// holding a commercial license may use this file in accordance with
// the Commercial License Agreement.
//
// This software is provided AS IS with NO WARRANTY OF ANY KIND,
// INCLUDING THE WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS
// FOR A PARTICULAR PURPOSE.
//
// Please contact the author if any conditions of this licensing are
// not clear to you.
//
// Author: Bernhard Kornberger, bkorn (at) geom.at
// http://www.geom.at

#pragma once

#include "common.h"
#include "Bbox2.h"
#include "Edge2.h"
#include "Segment2.h"
#include "UserPredicates.h"

#if GEOM_PSEUDO3D==GEOM_TRUE
	namespace GEOM_FADE25D {
#elif GEOM_PSEUDO3D==GEOM_FALSE
	namespace GEOM_FADE2D {
#else
	#error GEOM_PSEUDO3D is not defined
#endif


class Dt2; // Fwd
class ConstraintGraph2; // Fwd
class Triangle2; // Fwd
class Point2; // Fwd
class Visualizer2; // Fwd
/** \brief Zone2 is an exactly defined area of a triangulation
*
* \sa http://www.geom.at/example4-zones-defined-areas-in-triangulations/
* \sa http://www.geom.at/boolean-operations/
* \sa \ref createZone in the Fade2D class
*/


class CLASS_DECLSPEC Zone2
{
public:
/// @private
	Zone2(Dt2* pDt_,ZoneLocation zoneLoc_);
/// @private
	Zone2(Dt2* pDt_,ZoneLocation zoneLoc_,ConstraintGraph2* pConstraintGraph_);
/// @private
	Zone2(Dt2* pDt_,ZoneLocation zoneLoc_,const std::vector<ConstraintGraph2*>& vConstraintGraphs_);
/// @private
	Zone2(Dt2* pDt_,const std::vector<ConstraintGraph2*>& vConstraintGraphs_,ZoneLocation zoneLoc_,std::vector<Point2>& vStartPoints);

/** \brief Get the zone location
*
* \returns ZL_INSIDE if the zone applies to the triangles inside one or more ConstraintGraph2 objects@n
* ZL_OUTSIDE if the zone applies to the outside triangles@n
* ZL_GLOBAL if the zone applies (dynamically) to all triangles@n
* ZL_RESULT if the zone is the result of a set operation@n
* ZL_GROW if the zone is specified by a set of constraint graphs and an inner point@n
* \image html in_and_outside_zone.jpg "An ouside zone and in inside zone"
* \image latex in_and_outside_zone.eps "An ouside zone and in inside zone" width=12cm
*/
	ZoneLocation getZoneLocation() const;

/** \brief Convert a zone to a bounded zone
*
* \anchor convertToBoundedZone
* The mesh generation algorithms refine() and refineAdvanced() require
* a zone object that is bounded by constraint segments. This is always
* the case for zones with zoneLocation ZL_INSIDE but other types of
* zones may be unbounded. For convenience this method is provided to
* create a bounded zone from a possibly unbounded one.
*
* @return a pointer to a new Zone2 object with zoneLocation ZL_RESULT_BOUNDED
* or @e this if this->getZoneLocation() is ZL_INSIDE.
*/
	Zone2* convertToBoundedZone();

/** \brief Postscript visualization
*
* @param postscriptFilename is the name of the output file.
* @param bShowFull specifies if only the zone or the full triangulation shall be drawn
* @param bWithConstraints specifies if constraint edges shall be drawn
*
*/
	void show(const std::string& postscriptFilename,bool bShowFull,bool bWithConstraints) const;

/** \brief Postscript visualization
*
* @param pVisualizer is a pointer to an existing Visualizer2 object.
* @note You must call pVisualizer->writeFile() before program end
* @param bShowFull specifies if only the zone or the full triangulation shall be drawn
* @param bWithConstraints specifies if constraint edges shall be drawn
*/
	void show(Visualizer2* pVisualizer,bool bShowFull,bool bWithConstraints) const;

#if GEOM_PSEUDO3D==GEOM_TRUE
/** \brief Geomview visualization
*
* @param filename is the name of the output file.
* @param color is a string("red green blue alpha"), e.g., "1.0 0.0 0.0 1.0"*
*
*/
	void showGeomview(const std::string& filename,const std::string& color) const;
#endif

#if GEOM_PSEUDO3D==GEOM_TRUE


void analyzeAngles();
/**
 * Optimize Slopes, Valleys and Ridges
 */
void slopeValleyRidgeOptimization();

/**
 * Optimize Valleys and Ridges
 *
 * A Delaunay triangulation not unique when when 2 or more triangles
 * share a common circumcircle. As a consequence the four corners of
 * a rectangle can be triangulated in two different ways: Either the
 * diagonal proceeds from the lower left to the upper right corner
 * or it connects the other two corners. Both solutions are valid and
 * an arbitrary one is applied when points are triangulated. To improve
 * the repeatability and for reasons of visual appearance this method
 * unifies such diagonals such that they point from the lower left to
 * the upper right corner (or in horizontal direction).\n
 *
 * Moreover a Delaunay triangulation does not take the z-value into
 * account and thus valleys and ridges may be disturbed. The present
 * method flips diagonals such that they point from the lower left to
 * the upper right corner of a quad. And if the 2.5D lengths of the
 * diagonals are significantly different, then the shorter one is
 * applied.
 *
 * @param tolerance2D is 0 when only exact cases of more than 3 points
 * on a common circumcircle shall be changed. But in practice input
 * data can be disturbed by noise and tiny rounding errors such that
 * grid points are not exactly on a grid. The numeric error is computed
 * as \f$error=\frac{abs(diagonalA-diagonalB)}{max(diagonalA,diagonalB)}\f$.
 * and \p tolerance2D is an upper threshold to allow modification despite
 * such tiny inaccuracies.
 * @param lowerThreshold25D is used to take also the heights of the
 * involved points into account. For example, the points\n
 * \n
 * Point_2 a(0,0,0);\n
 * Point_2 b(10,0,0);\n
 * Point_2 c(10,10,0);\n
 * Point_2 d(0,10,1000);\n
 * \n
 * can form the triangles (a,b,c) and (a,c,d) or the triangles (a,b,d)
 * and (d,b,c) but (a,c) is obviousy the better diagonal because the
 * points a,b,c share the same elevation while d is at z=1000.
 * Technically spoken, the diagonal with the smaller 2.5D-length is
 * applied if the both, the 2D error is below \p tolerance2D and the
 * 2.5D error is above \p lowerThreshold25D. The 2.5D
 * criterion has priority over the 2D criterion.
 *
 */
	void optimizeValleysAndRidges(double tolerance2D,double lowerThreshold25D);
#endif

/**
 * Unify Grid
 *
 * A Delaunay triangulation not unique when when 2 or more triangles
 * share a common circumcircle. As a consequence the four corners of
 * a rectangle can be triangulated in two different ways: Either the
 * diagonal proceeds from the lower left to the upper right corner
 * or it connects the other two corners. Both solutions are valid and
 * an arbitrary one is applied when points are triangulated. To improve
 * the repeatability and for reasons of visual appearance this method
 * unifies such diagonals to point from the lower left to the upper
 * right corner (or in horizontal direction).
 *
 * @param tolerance is 0 when only exact cases of more than 3 points
 * on a common circumcircle shall be changed. But in practice input
 * data can be disturbed by noise and tiny rounding errors such that
 * grid points are not exactly on a grid. The numeric error is computed
 * as \f$error=\frac{abs(diagonalA-diagonalB)}{max(diagonalA,diagonalB)}\f$.
 * and \p tolerance is an upper threshold to allow modification despite
 * such tiny inaccuracies. Use with caution, such flips break the
 * empty circle property and this may or may not fit your setting.
 */
void unifyGrid(double tolerance);

/// @private
	bool assignDt2(Dt2* pDt_);

/** \brief Get the triangles of the zone.
*
* This command fetches the existing triangles of the zone.
*
* @note Fade_2D::void applyConstraintsAndZones() must be called after
* the last * insertion of points and constraints. Otherwise the result
* won't be valid.
*/
	void getTriangles(std::vector<Triangle2*>& vTriangles_) const;

/** \brief Get the vertices of the zone.
*
*/
	void getVertices(std::vector<Point2*>& vVertices_) const;


/** Statistics
 *
 * Prints statistics to stdout.
 */
void statistics(const std::string& s) const;


/** \brief Get the associated constraint
* @return a pointer to the ConstraintGraph2 object which defines the zone.@n
* or NULL for ZL_RESULT-, ZL_GROW and ZL_GLOBAL_-zones.
*/
	ConstraintGraph2* getConstraintGraph() const;

/** \brief Get the number of triangles
* @warning This method is fast but O(n), so don't call it frequently in a loop.
*
*/
	size_t getNumberOfTriangles() const;


/** \brief Get the associated constraint graphs
*
*/
	void getConstraintGraphs(std::vector<ConstraintGraph2*>& vConstraintGraphs_) const;

/// @private
	Dt2* getDelaunayTriangulation() const;

/** \brief Get a the number of ConstraintGraph2 objects
*
* A Zone2 object might be defined by zero, one or more ConstraintGraph2 objects.
*/
	size_t numberOfConstraintGraphs() const;

/** \brief Development function
 */
	void debug(std::string name="");

/** \brief Compute the bounding box
 */
	Bbox2 getBoundingBox() const;

/** \brief Compute the boundary edges of the zone
 */
	void getBoundaryEdges(std::vector<Edge2>& vEdges) const;

/** \brief Compute the boundary segments of the zone
 */
	void getBoundarySegments(std::vector<Segment2>& vSegments) const;

/** \brief Get 2D Area
 *
 * Returns the 2D area of the zone.
 *
 * \if SECTION_FADE25D
 * Note: The getArea() method is deprecated and replaced by getArea2D()
 * and getArea25D()
 * \else
 * Note: The getArea() method is deprecated and replaced by getArea2D()
 * to keep the names consistent.
 * \endif
 */
	double getArea2D() const;


#if GEOM_PSEUDO3D==GEOM_TRUE
/** \brief Get 2.5D Area
 *
 * Returns the 2.5D area of the zone.
 *
 * Note: The getArea() method is deprecated and replaced by getArea2D()
 * and getArea25D()
 */
	double getArea25D() const;
#endif

/** \brief Get border edges
 * @return: the CCW oriented border edges of the zone
 */
	void getBorderEdges(std::vector<Edge2>& vBorderEdgesOut) const;


protected:
/// @private
	Zone2(const Zone2&);
/// @private
	void getTriangles_RESULT(std::vector<Triangle2*>& vTriangles) const;
/// @private
	void initWorkspace(bool bInside,std::vector<Triangle2*>& vWorkspace) const;
/// @private
	void bfsFromWorkspace(std::vector<Triangle2*>& vWorkspace,std::vector<Triangle2*>& vTriangles) const;
/// @private
	Zone2* ctbz_treatCC(std::vector<Triangle2*>& vOneCC);
/// @private
	void optimize(std::vector<Triangle2*>& vT,std::vector<Triangle2*>& vChangedT);
	// Data
	Dt2* pDt;
	ZoneLocation zoneLoc;
	//Zone2* pInputZone0;
	//Zone2* pInputZone1;
	CLASS_DECLSPEC
	friend Zone2* zoneUnion(Zone2* pZone0,Zone2* pZone1);
	CLASS_DECLSPEC
	friend Zone2* zoneIntersection(Zone2* pZone0,Zone2* pZone1);
	CLASS_DECLSPEC
	friend Zone2* zoneDifference(Zone2* pZone0,Zone2* pZone1);
	CLASS_DECLSPEC
	friend Zone2* zoneSymmetricDifference(Zone2* pZone0,Zone2* pZone1);
	CLASS_DECLSPEC
	friend Zone2* peelOffIf(Zone2* pZone, UserPredicateT* pPredicate,bool bVerbose);

private:
#ifdef _WIN32
#pragma warning(push)
#pragma warning(disable:4251)
#endif
	std::vector<ConstraintGraph2*> vConstraintGraphs;
	std::vector<Point2> vStartPoints;
	std::vector<Zone2*> vInputZones;
#ifdef _WIN32
#pragma warning(pop)
#endif

};

// Free functions

/** \brief Compute the union of two zones
* @return a new zone containing the union of the triangles of *pZone0 and *pZone1
*/
CLASS_DECLSPEC
Zone2* zoneUnion(Zone2* pZone0,Zone2* pZone1);
/** \brief Compute the intersection of two zones
* @return a new zone containing the intersection of *pZone0 and *pZone1
*/
CLASS_DECLSPEC
Zone2* zoneIntersection(Zone2* pZone0,Zone2* pZone1);
/** \brief Compute the difference of two zones
* @return a new zone containing the triangles of *pZone0 minus the ones of *pZone1
*/
CLASS_DECLSPEC
Zone2* zoneDifference(Zone2* pZone0,Zone2* pZone1);
/** \brief Compute the symmetric difference of two zones
* @return a new zone containing the triangles that are present in one of
* the zones but not in the other one.
*/
CLASS_DECLSPEC
Zone2* zoneSymmetricDifference(Zone2* pZone0,Zone2* pZone1);





} // (namespace)
