#ifndef POLYSHAPES_H
#define POLYSHAPES_H

#include <basicShape.h>
#include <map>


class polyShapes {


protected:
    
    /* ----------------------  Private members ----------------------  */

    // List of shapes
    std::vector<basicShape*> _shapes;

    // Overall bounding box
    Vector3 _bbox_min,
	_bbox_max;


    // Boundary weigths. point over boundary preference
    std::map<std::string, int> _bdWeight;
    

    

    /* ----------------------  Public member functions ----------------------  */

public:

    // Constructors and destructors

    // Default constructor
    polyShapes(const std::string& dictName);

    // Constructor with dictionary and shape name (only one shape)
    polyShapes(const std::string& dictName, const std::string& shapeName);    

    // Default destructor
    ~polyShapes();



    /* // Add shape to list */
    /* void addShape( const shape& s  ); */

    // Check if point is inside the domain
    const bool locatePoint( const Vector3& v ) const;

    // Bounding box
    const std::tuple<Vector3, Vector3> boundingBox() const;

    // Boundaries list
    const std::vector<std::string> boundaryNames() const;

    // Check the patch that contains a point
    const std::string pointOverBoundary(const Vector3& v, const double& tol = 0) const;

    // Value factor
    const double value( const Vector3& v ) const;


};

#endif // POLYSHAPES_H
