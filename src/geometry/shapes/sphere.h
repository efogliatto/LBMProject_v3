#ifndef SPHERE_H
#define SPHERE_H

#include <basicShape.h>

class sphere : public basicShape {

protected:
    
    // Sphere radius
    double _radius;

    // Centre
    Vector3 _origin;


    /* ----------------------  Public member functions ----------------------  */

public:

    // Constructors and destructors

    // Default constructor
    sphere();

    // Constructor with name
    sphere(const std::string& name);

    // Default destructor
    ~sphere();



    // Access members

    // Shape volume
    const double& vol() const;

    // Shape area
    const double& area() const;

    // Shape name
    const std::string& name() const;

    // Bounding box limits
    const std::tuple<Vector3, Vector3> boundingBox() const;

    // Check if point is inside the shape
    virtual const bool locatePoint(const Vector3& v) const;

    // Boundary names
    const std::vector<std::string> boundaryNames() const;

    // Check the boundary that contains a point
    const std::string pointOverBoundary(const Vector3& v, const double& tol = 0) const;



    // Member modification

    // Reading properties
    void readProperties(const std::string& dictName);

};


#endif // SPHERE_H
