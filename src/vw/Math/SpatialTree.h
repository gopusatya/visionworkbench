#ifndef __VW_MATH_SPATIAL_TREE_H__
#define __VW_MATH_SPATIAL_TREE_H__

// Note that this class is not intended to implement a Binary
// Space Partition (BSP)...

// STL includes:
#include <iostream>
#include <list>

#include <vw/Math/Vector.h>
#include <vw/Math/BBox.h>

namespace vw {
namespace math {

  class GeomPrimitive {
  public:
    // distance() must be implemented to call SpatialTree::closest()
    virtual double distance(const Vector<double> &point) const;
    // contains() must be implemented to call SpatialTree::contains()
    virtual bool contains(const Vector<double> &point) const;
    // intersects() must be implemented to call SpatialTree::overlap_pairs()
    virtual bool intersects(const GeomPrimitive *prim) const;
    virtual const BBox<double> &bounding_box() const = 0;
  };

  class SpatialTree {
  public:
    typedef BBox<double> BBoxT;
    typedef Vector<double> VectorT;

    struct PrimitiveListElem {
      PrimitiveListElem() {
        next = 0;
        prim = 0;
        forced_this_level = false;
      }
      PrimitiveListElem *next;
      GeomPrimitive *prim;
      bool forced_this_level;
    };
    
    struct SpatialTreeNode {
      SpatialTreeNode(int num_quadrants) {
        m_quadrant = new SpatialTreeNode*[num_quadrants];
        for (int i = 0; i < num_quadrants; i++)
          m_quadrant[i] = 0;
        m_primitive_list = 0;
        m_num_primitives = 0; // mostly for debugging
        m_is_split = false;
      }
      ~SpatialTreeNode() {
        delete[] m_quadrant;
        m_quadrant = 0;   
      }
      bool is_split() { return m_is_split; }
      BBoxT &bounding_box() { return m_bbox; }
      BBoxT m_bbox;
      PrimitiveListElem *m_primitive_list;
      int m_num_primitives; // mostly for debugging
      SpatialTreeNode **m_quadrant;
      bool m_is_split;
    };

    SpatialTree(BBoxT bbox);
    SpatialTree(int num_primitives, GeomPrimitive **prims, int max_create_level = -1);
    ~SpatialTree();
    void add(GeomPrimitive *prim, int max_create_level = -1);
    const BBoxT &bounding_box() const { return m_root_node->m_bbox; }
    GeomPrimitive *closest(const VectorT &point, double distance_threshold = -1);
    GeomPrimitive *contains(const VectorT &point);
    void contains(const VectorT &point, std::list<GeomPrimitive*> &prims);
    void overlap_pairs(std::list<std::pair<GeomPrimitive*, GeomPrimitive*> > &overlaps);
    void print(std::ostream &os = std::cout);
    //NOTE: this can only write a 2D projection (because VRML is 3D)
    void write_vrml(char *fn, int level = -1);
    void write_vrml(std::ostream &os = std::cout, int level = -1);
    bool check(std::ostream &os = std::cerr);
  private:
    int m_dim;
    int m_num_quadrants;
    SpatialTreeNode *m_root_node;
  };

}} // namespace vw::math

#endif // _SPATIAL_TREE_H_
