/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2009 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_SHAPE_CUTTER_UTIL_H
#define HK_SHAPE_CUTTER_UTIL_H

#include <Physics/Collide/Shape/Convex/hkpConvexShape.h>
#include <Physics/Collide/Shape/Convex/ConvexVertices/hkpConvexVerticesConnectivity.h>
#include <Common/Base/Types/Geometry/hkGeometry.h>


class hkpConvexShape;
class hkpConvexVerticesShape;
class hkpCollisionDispatcher;


	/// This small utility class allows for testing whether two given shapes are 'connected', i.e. whether
	/// or not the smallest distance between them is below a given threshold or not.
class hkpShapeConnectedCalculator : public hkReferencedObject
{
    public:

        HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_COLLIDE);

			/// Constructor.
        hkpShapeConnectedCalculator(hkpCollisionDispatcher* collisionDispatcher, hkReal maxDistanceForConnection);

            /// Returns true if the two shapes are 'connected'. The two transforms are local to world transforms for
            /// each of the respective shapes.
        virtual hkBool isConnected( const hkpShape* a, const hkTransform& transA, const hkpShape* b, const hkTransform& transB );

	protected:

            // Works out the closest distance between two transformed shapes.
        static hkBool HK_CALL calculateClosestDistance( hkpCollisionDispatcher* dispatcher, const hkpShape* a, const hkTransform& transA, const hkpShape* b, const hkTransform& transB, hkReal maxDistance, hkReal& distanceOut);

	protected:

        hkpCollisionDispatcher* m_dispatcher;

            /// The distance between must be less than m_distance to be 'connected'
        hkReal m_maxDistanceForConnection;
};


	/// Utility class that can be used to cut shapes with a plane.
class hkpShapeCutterUtil
{
	public:

		//
		//		CSG operations
		//

        HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_COLLIDE, hkpShapeCutterUtil );

             /// Creates a new shape, which is shape cut by the plane 'plane'. The cut shape returned is the shape
            /// on the 'inside' of the plane (ie the opposite side to the normal)
            /// If the resulting shape is empty, HK_NULL will be returned
			/// If the input shape is a sphere, capsule or a cylinder, it will get \a extraConvexRadiusForImplicitShapes as its extra convex radius.
            /// If the shape isn't cut at all it will be returned (with reference count incremented)
        static const hkpShape* HK_CALL cut(const hkpShape* shape, const hkVector4& plane, hkReal extraConvexRadiusForImplicitShapes = 0.001f );



            /// Creates potentially two new shapes, one on either side of the plane.
            /// \a ShapeAOut will be the shape on the inside of the plane (opposite side to the normal), \a shapeBOut will be on the outside of the plane
            /// If the resulting shape is empty, HK_NULL will be returned
			/// If the shape was a sphere, capsule or a cylinder, it will get \a extraConvexRadius as its extra convex radius.
            /// If the shape isn't cut at all it will be returned (with reference count incremented)
        static void HK_CALL cut(const hkpShape* shapeIn, const hkVector4& planeIn, hkReal extraConvexRadiusForImplicitShapes, const hkpShape** shapeAOut, const hkpShape** shapeBOut);

			/// Input into subtractConvexShape
		struct SubtractShapeInput
		{
			SubtractShapeInput(): m_dispatcher(HK_NULL), m_subtractShape(HK_NULL), m_extraConvexRadiusForImplicitShapes(0.001f), m_allowedPentration(.05f){ m_transform.setIdentity(); }

			hkpCollisionDispatcher* m_dispatcher;			///
			const hkpConvexVerticesShape* m_subtractShape;	///
			hkTransform m_transform;						///
			hkReal m_extraConvexRadiusForImplicitShapes;	///
			hkReal m_allowedPentration;						///
		};

			/// Takes an input shape and subtracts a convex shape 
		static const hkpShape* HK_CALL subtractConvexShape( const hkpShape* shape, const SubtractShapeInput& input );
 
			
			/// Input to intersectWithConvexShape()
		struct IntersectShapeInput
		{
			IntersectShapeInput(): m_planeEquations(HK_NULL), m_numPlaneEquations(0), m_expandPlaneEquations(0.0f), m_extraConvexRadiusForImplicitShapes(0.001f), m_planesUsedOut(HK_NULL) { m_transform.setIdentity(); }

			const hkVector4* m_planeEquations;			///
			int m_numPlaneEquations;					///
			hkTransform m_transform;					///
			hkReal m_expandPlaneEquations;				///
			hkReal m_extraConvexRadiusForImplicitShapes; 	///
			hkBool*		m_planesUsedOut;				///
		};



			/// Intersects a physics shape with the plane equations of a convex vertices shape
		static const hkpShape* HK_CALL intersectWithConvexShape( const hkpShape* shape, const IntersectShapeInput& input );


		//
		//	Convertion functions
		//


			/// Will convert input shape into convex vertices shape if it can. If shape is a convex vertices shape,
            /// will return it with a ref on it. If it is not possible to convert, will return HK_NULL.
			/// If the shape was a sphere, capsule or a cylinder, it will get \a extraConvexRadiusForImplicitShapes as its extra convex radius.
            /// Note this method only converts 'terminal' convex shapes such as hkpBoxShape etc, not hkpTransformShape or containers
        static const hkpConvexVerticesShape* HK_CALL ensureConvexVerticesShape(const hkpConvexShape* shape, hkReal extraConvexRadius = 0.001f);

			/// For a given sphere radius will return the appropriate amount of rows based on
			/// a desired edge length for the faces, and a maxRadius.
			/// If the sphere is smaller than the edge size, will return the appropriate number of
			/// rows for a minimal sphere.
		static int HK_CALL approxSphereRows(hkReal edgeSize, int maxFaces, hkReal radius);

            /// Create a compound shape from the sub shapes with transforms
        static const hkpShape* HK_CALL createCompound(const hkpConvexShape*const* shapes, const hkTransform* transforms, int numShapes);

            /// Create a hkpConvexVerticesShape from an aabb
        static hkpConvexVerticesShape* HK_CALL createAabbConvexVerticesShape(const hkAabb& aabb, hkReal convexRadius);

            /// Create a sphere as a hkpConvexVerticesShape - can then be cut
		static hkpConvexVerticesShape* HK_CALL createSphereConvexVerticesShape(hkReal radius, hkReal extraConvexRadius, int numRows = 10);

			/// Create a cylinder sitting on the z = 0 plane, with height specified and the number of segments making up the surface.
		static hkpConvexVerticesShape* HK_CALL createCylinderConvexVerticesShape(hkReal radius, hkReal extraConvexRadius, hkReal height, int numSegs, const hkTransform& trans);

			/// Create a cylinder with start and end points, and radius with numSegs dividing it up.
		static hkpConvexVerticesShape* HK_CALL createCylinderConvexVerticesShape(hkReal radius, hkReal extraConvexRadius, const hkVector4& v0, const hkVector4& v1, int numSegs);

			/// Create a capsule with the given bottom and top vertices and radius using the specified segmenting.
		static hkpConvexVerticesShape* HK_CALL createCapsuleConvexVerticesShape(const hkVector4& top, const hkVector4& bottom, hkReal radius, hkReal  extraConvexRadius, int numSides = 6, int numHeightSegments = 1);

			/// Returns a convex shape which has the new transform. Will try and use the fastest transform shape,
			/// or no transform at all depending on the transform. <js.todo move this function to hkpTransformCollapseUtil, also try to convert shapes in place without using an extra transform shape (hkCapsuleShapes can easily be transformed)
        static const hkpShape* HK_CALL transformConvexShape(const hkpConvexShape* shape, const hkTransform& transform);

            /// Returns the shape transformed by transform. Use 'transformShape' to find a reasonably optimal transform.
            /// This function does not try and combine transforms - but will normally append a hkpTransformShape in front of shape unless
            /// the transform is the identity <js.todo move this function to hkpTransformCollapseUtil, rename hkpTransformCollapseUtil to hkpTransformShapeUtil
        static const hkpShape* HK_CALL transformNonConvexShape(const hkpShape* shape,const hkTransform& transform);

            /// Returns a shape that has been transformed by shape. Will try and use the fastest setup, depending on the shape and the transform <js.todo move this function to hkpTransformCollapseUtil, rename hkpTransformCollapseUtil to hkpTransformShapeUtil
        static const hkpShape* HK_CALL transformShape(const hkpShape* shape, const hkTransform& transform);

            /// Find all of the convex shapes in a shape hierarchy.
            /// The transforms are the local to reference transforms for each of the shapes.
            /// NOTE! This method will not work properly with shape hierarchies that have shape collections which do not
            /// return actual shape pointers (as opposed to storage being in the ShapeBuffer) - as the buffers will go
            /// out of scope before the method returns.
        static hkResult HK_CALL flattenIntoConvexShapes(const hkpShape* shape, const hkTransform& parentToReference, hkArray<const hkpConvexShape*>& shapesOut, hkArray<hkTransform>& localToReferenceTransformsOut );

            /// Partitions the shapes, based on which shapes are connected
        static void HK_CALL findConnectedIslands(hkpShapeConnectedCalculator* connected, const hkArray<const hkpConvexShape*>& shapes, const hkArray<hkTransform>& transforms, hkArray<int>& partitionSizes, hkArray<int>& partitions);

            /// Returns -1 if shape is inside, 0 on and +1 if outside plane
        static int HK_CALL findShapeSideOfPlane(const hkpShape* shape, const hkVector4& plane);

			/// Returns -1 if shape is inside, 0 if on and +1 if outside plane
		static int HK_CALL findShapeSideOfPlane(const hkpConvexShape* shapeIn, const hkVector4& plane);


		static const hkpShape* HK_CALL _subtractConvexShape( const hkpShape* shape, const hkVector4* planes, int numPlanes, hkReal extraConvexRadiusForImplicitShapes = 0.001f );


};


#endif // HK_SHAPE_PLANE_CUT_UTIL_H

/*
* Havok SDK - NO SOURCE PC DOWNLOAD, BUILD(#20090704)
* 
* Confidential Information of Havok.  (C) Copyright 1999-2009
* Telekinesys Research Limited t/a Havok. All Rights Reserved. The Havok
* Logo, and the Havok buzzsaw logo are trademarks of Havok.  Title, ownership
* rights, and intellectual property rights in the Havok software remain in
* Havok and/or its suppliers.
* 
* Use of this software for evaluation purposes is subject to and indicates
* acceptance of the End User licence Agreement for this product. A copy of
* the license is included with this software and is also available at www.havok.com/tryhavok.
* 
*/
