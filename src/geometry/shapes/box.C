#include <box.h>
#include <cmath>

using namespace std;



/* ----------------------  Public member functions ----------------------  */


// Constructors and destructors

// Default constructor
box::box() : basicShape() {


    // Assign boundary weights
    dictionary dict("properties/latticeProperties");

    // XY box
    if ( _bbox_min.z() == _bbox_max.z() ) {

	_bdw[_name + "_X0"] = (int)dict.lookUpEntry<double>("boundaryWeights/" + _name + "_X0");
	_bdw[_name + "_X1"] = (int)dict.lookUpEntry<double>("boundaryWeights/" + _name + "_X1");
	_bdw[_name + "_Y0"] = (int)dict.lookUpEntry<double>("boundaryWeights/" + _name + "_Y0");
	_bdw[_name + "_Y1"] = (int)dict.lookUpEntry<double>("boundaryWeights/" + _name + "_Y1");
	
    }

    else {

	// XZ box
	if ( _bbox_min.y() == _bbox_max.y() ) {

	    _bdw[_name + "_X0"] = (int)dict.lookUpEntry<double>("boundaryWeights/" + _name + "_X0");
	    _bdw[_name + "_X1"] = (int)dict.lookUpEntry<double>("boundaryWeights/" + _name + "_X1");
	    _bdw[_name + "_Z0"] = (int)dict.lookUpEntry<double>("boundaryWeights/" + _name + "_Z0");
	    _bdw[_name + "_Z1"] = (int)dict.lookUpEntry<double>("boundaryWeights/" + _name + "_Z1");
	
	}


	else {

	    // YZ box
	    if ( _bbox_min.z() == _bbox_max.z() ) {

		_bdw[_name + "_Y0"] = (int)dict.lookUpEntry<double>("boundaryWeights/" + _name + "_Y0");
		_bdw[_name + "_Y1"] = (int)dict.lookUpEntry<double>("boundaryWeights/" + _name + "_Y1");
		_bdw[_name + "_Z0"] = (int)dict.lookUpEntry<double>("boundaryWeights/" + _name + "_Z0");
		_bdw[_name + "_Z1"] = (int)dict.lookUpEntry<double>("boundaryWeights/" + _name + "_Z1");
	
	    }

	}	

    }



}

// Default constructor
box::box(const string& name) : basicShape() {

    // Assign name
    _name = name;

    // Assign boundary weights
    dictionary dict("properties/latticeProperties");

    // XY box
    if ( _bbox_min.z() == _bbox_max.z() ) {

	_bdw[_name + "_X0"] = (int)dict.lookUpEntry<double>("boundaryWeights/" + _name + "_X0");
	_bdw[_name + "_X1"] = (int)dict.lookUpEntry<double>("boundaryWeights/" + _name + "_X1");
	_bdw[_name + "_Y0"] = (int)dict.lookUpEntry<double>("boundaryWeights/" + _name + "_Y0");
	_bdw[_name + "_Y1"] = (int)dict.lookUpEntry<double>("boundaryWeights/" + _name + "_Y1");
	
    }

    else {

	// XZ box
	if ( _bbox_min.y() == _bbox_max.y() ) {

	    _bdw[_name + "_X0"] = (int)dict.lookUpEntry<double>("boundaryWeights/" + _name + "_X0");
	    _bdw[_name + "_X1"] = (int)dict.lookUpEntry<double>("boundaryWeights/" + _name + "_X1");
	    _bdw[_name + "_Z0"] = (int)dict.lookUpEntry<double>("boundaryWeights/" + _name + "_Z0");
	    _bdw[_name + "_Z1"] = (int)dict.lookUpEntry<double>("boundaryWeights/" + _name + "_Z1");
	
	}


	else {

	    // YZ box
	    if ( _bbox_min.z() == _bbox_max.z() ) {

		_bdw[_name + "_Y0"] = (int)dict.lookUpEntry<double>("boundaryWeights/" + _name + "_Y0");
		_bdw[_name + "_Y1"] = (int)dict.lookUpEntry<double>("boundaryWeights/" + _name + "_Y1");
		_bdw[_name + "_Z0"] = (int)dict.lookUpEntry<double>("boundaryWeights/" + _name + "_Z0");
		_bdw[_name + "_Z1"] = (int)dict.lookUpEntry<double>("boundaryWeights/" + _name + "_Z1");
	
	    }

	}	

    }

    
}



// Default destructor
box::~box() {}



// Access members

// Shape volume
const double& box::vol() const{
    return basicShape::vol();
}

// Shape area
const double& box::area() const {
    return basicShape::area();
}

// Shape name
const string& box::name() const {
    return basicShape::name();
}

// Bounding box limits
const tuple<Vector3, Vector3> box::boundingBox() const {
    return basicShape::boundingBox();
}


// Check if point is inside(outside) the shape
const bool box::locatePoint(const Vector3& v) const {

    bool is_inside = ( v.x() >= _bbox_min.x() ) && ( v.x() <= _bbox_max.x() )  &&  ( v.y() >= _bbox_min.y() ) && ( v.y() <= _bbox_max.y() ) && ( v.z() >= _bbox_min.z() ) && ( v.z() <= _bbox_max.z() );
    
    return is_inside == _inside;
}




// Boundary names
const vector<string> box::boundaryNames() const {

    vector<string> names;

    if(!_inside) {
	names.push_back(_name);
    }
    else {

	if ( _bbox_min.x() != _bbox_max.x() ){
	    names.push_back(_name + "_X0");
	    names.push_back(_name + "_X1");
	}
	if ( _bbox_min.y() != _bbox_max.y() ){
	    names.push_back(_name + "_Y0");
	    names.push_back(_name + "_Y1");
	}
	if ( _bbox_min.z() != _bbox_max.z() ){
	    names.push_back(_name + "_Z0");
	    names.push_back(_name + "_Z1");
	}

    }

    return names;

}





// Member modification

// Reading properties
void box::readProperties(const string& dictName) {

    dictionary dict(dictName);

    // Min bounding box
    _bbox_min = dict.lookUpEntry<Vector3>( _name + "/min" );

    // Min bounding box
    _bbox_max = dict.lookUpEntry<Vector3>( _name + "/max" );


    // Inside
    string inside = dict.lookUpEntry<string>( _name + "/inside" );
    if(inside.find("true") != string::npos) { _inside = true;  }
    else{ _inside = false; }

    
    // Set properties
    _volume = fabs(  (_bbox_max.x() - _bbox_min.x() )  *  (_bbox_max.y() - _bbox_min.y() )  *  (_bbox_max.z() - _bbox_min.z() )  );

}






// Check the patch that contains a point
const string box::pointOverBoundary(const Vector3& v, const double& tol) const {

    string patch;
    vector<string> vpatch;

    // Check over each plane

    // YZ
    if ( _bbox_min.x() == _bbox_max.x() ){


	// Y0
	if (  (v.y() == _bbox_min.y())  &&  (v.z() >= _bbox_min.z())  &&  (v.z() <= _bbox_max.z())  ) { patch = _name + "_Y0"; }

	// Y1
	if (  (v.y() == _bbox_max.y())  &&  (v.z() >= _bbox_min.z())  &&  (v.z() <= _bbox_max.z())  ) { patch = _name + "_Y1"; }

	// Z0
	if (  (v.z() == _bbox_min.z())  &&  (v.y() >= _bbox_min.y())  &&  (v.y() <= _bbox_max.y())  ) { patch = _name + "_Z0"; }

	// Z1
	if (  (v.z() == _bbox_max.z())  &&  (v.y() >= _bbox_min.y())  &&  (v.y() <= _bbox_max.y())  ) { patch = _name + "_Z1"; }
	
    }


    
    // XZ
    if ( _bbox_min.y() == _bbox_max.y() ){


	// X0
	if (  (v.x() == _bbox_min.x())  &&  (v.z() >= _bbox_min.z())  &&  (v.z() <= _bbox_max.z())  ) { patch = _name + "_X0"; }

	// X1
	if (  (v.x() == _bbox_max.x())  &&  (v.z() >= _bbox_min.z())  &&  (v.z() <= _bbox_max.z())  ) { patch = _name + "_X1"; }

	// Z0
	if (  (v.z() == _bbox_min.z())  &&  (v.x() >= _bbox_min.x())  &&  (v.x() <= _bbox_max.x()) ) { patch = _name + "_Z0"; }

	// Z1
	if (  (v.z() == _bbox_max.z())  &&  (v.x() >= _bbox_min.x())  &&  (v.x() <= _bbox_max.x()) ) { patch = _name + "_Z1"; }

	
    }


    
    // XY
    if ( _bbox_min.z() == _bbox_max.z() ){

	// X0
	if (  (v.x() == _bbox_min.x())  &&  (v.y() >= _bbox_min.y())  &&  (v.y() <= _bbox_max.y())  ) { vpatch.push_back( _name + "_X0" ); }

	// X1
	if (  (v.x() == _bbox_max.x())  &&  (v.y() >= _bbox_min.y())  &&  (v.y() <= _bbox_max.y()) ) { vpatch.push_back( _name + "_X1" ); }

	// Y0
	if (  (v.y() == _bbox_min.y())  &&  (v.x() >= _bbox_min.x())  &&  (v.x() <= _bbox_max.x()) ) { vpatch.push_back( _name + "_Y0" ); }

	// Y1
	if (  (v.y() == _bbox_max.y())  &&  (v.x() >= _bbox_min.x())  &&  (v.x() <= _bbox_max.x()) ) { vpatch.push_back( _name + "_Y1" ); }
	
    }



    if( vpatch.size() == 1 ) {
	    
	if( _bdw.at(vpatch[0]) >= 0 ) {  patch = vpatch[0]; }
	    
    }
	
    else {

	if( vpatch.size() == 2 ) {
		
	    if( _bdw.at(vpatch[0]) > _bdw.at(vpatch[1])  ) {
		if( _bdw.at(vpatch[0]) >= 0 ) {  patch = vpatch[0];  }
	    }
	    else {
		if( _bdw.at(vpatch[1]) >= 0 ) {  patch = vpatch[1]; }
	    }
	    
	}	    
	    
    }    


    if(!_inside && !patch.empty()) {patch = _name;}


    return patch;
}




// Value
const double box::value(const Vector3& v) const {
    return 1.0;
}

// Value
const double box::value(const Vector3& v, const double& val) const {
    return val;
}
