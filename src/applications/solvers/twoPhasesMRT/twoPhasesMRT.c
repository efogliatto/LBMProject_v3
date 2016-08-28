#include <stdio.h>
#include <twoPhasesFields.h>
#include <cahnHilliardCollision.h>
#include <liangCollision.h>
#include <timeOptions.h>
#include <density.h>
#include <pressureWithU.h>
#include <chemicalPotential.h>
#include <orderParameter.h>

int main() {

    printf("     -------------  \n");
    printf("     | -   |   - |  \n");
    printf("     |   - | -   |  \n");
    printf("     |<----o---->|       Two Phases - Lattice-Boltzmann solver\n");
    printf("     |   - | -   |  \n");
    printf("     | -   |   - |  \n");
    printf("     -------------  \n");
    
    // Simulation properties
    struct solverInfo info = readBasicInfo();

    
    // Read Fields
    struct twoPhasesFields fields;

    // Neighbours indices
    fields.nb = readNeighbours(&info);
    printf("\nReading neighbour indices\n");
    
    // Order parameter
    fields.phi     = readScalarField("phi", &info);
    fields.phi_old = readScalarField("phi", &info);
    printf("\nReading field phi\n");

    // Chemical potential
    fields.muPhi = readScalarField("muPhi", &info);
    printf("\nReading field muPhi\n");

    // Pressure
    fields.p = readScalarField("p", &info);
    printf("\nReading field p\n");

    // Density
    fields.rho = readScalarField("rho", &info);
    printf("\nReading field rho\n");

    // Velocity
    fields.U     = readVectorField("U", &info);
    fields.U_old = readVectorField("U", &info);
    printf("\nReading field U\n");

    // Cahn-Hilliard field
    fields.h = readPdfField("h", &info);
    printf("\nReading field h\n");

    // Navier-Stokes field
    fields.g = readPdfField("g", &info);
    fields.swp = readPdfField("g", &info);
    printf("\nReading field g\n\n\n");
    



    // Advance in time. Collide, stream, update and write
    while( updateTime(&info) ) {
	
    	// Collide h
    	cahnHilliardCollision(&fields, &info);

    	// Collide g
    	liangCollision(&fields, &info);

	// Swap fields
	swap( &fields, &info, fields.h );
	swap( &fields, &info, fields.g );
	

	// Update macroscopic fields

	// Order parameter
	orderParameter( &fields, &info, fields.phi );	
	
	// Chemical potential
	chemicalPotential( &fields, &info, fields.muPhi );	
	
	// Density
	density( &fields, &info, fields.rho );

	// Velocity
	velocity( &fields, &info, fields.U );

	// Pressure
	pressureWithU( &fields, &info, fields.p );

	// Old values
	{

	    unsigned int id, k;

	    for( id = 0 ; id < info.lattice.nlocal ; id++ ) {

		fields.phi_old[id] = fields.phi[id];
		
		for( k = 0 ; k < 3 ; k++ ) {

		    fields.U_old[id][k] = fields.U[id][k];

		}

	    }
	    
	}
	
	
    	// Write fields
    	if( writeFlag(&info) ) {

	    printf("Time = %.2f\n", info.time.current);
	    printf("Elapsed time = %.2f seconds\n\n", elapsed(&info) );
	    
    	    // ScalarFields
    	    writeScalarField("phi", fields.phi, &info);
    	    writeScalarField("muPhi", fields.muPhi, &info);
    	    writeScalarField("rho", fields.rho, &info);
    	    writeScalarField("p", fields.p, &info);

    	    // Vector fields
    	    writeVectorField("U", fields.U, &info);

    	    // Pdf fields
    	    writePdfField("h", fields.h, &info);
    	    writePdfField("g", fields.g, &info);
	    
    	}

    }


    
    // Print info
    printf("\n  Finished in %.2f seconds \n\n", elapsed(&info) );
    
    return 0;
    
}
