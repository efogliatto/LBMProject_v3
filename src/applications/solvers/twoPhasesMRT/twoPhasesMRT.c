#include <stdio.h>
#include <twoPhasesFields.h>
#include <cahnHilliardCollision.h>
#include <liangCollision.h>
#include <timeOptions.h>
#include <density.h>
#include <velocity.h>
#include <pressureWithU.h>
#include <chemicalPotential.h>
#include <orderParameter.h>
#include <syncScalarField.h>
#include <syncPdfField.h>
#include <lbstream.h>
#include <writeScalarField.h>
#include <writeVectorField.h>
#include <writePdfField.h>

int main( int argc, char **argv ) {

    int pid, world;
    
    // Initialize mpi
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD,&pid);
    MPI_Comm_size(MPI_COMM_WORLD,&world);
    
    if(pid == 0) {
	printf("                    \n");	
	printf("     o-----o-----o  \n");
	printf("     | -   |   - |  \n");
	printf("     |   - | -   |  \n");
	printf("     o<----o---->o       Two Phases - Lattice-Boltzmann solver\n");
	printf("     |   - | -   |  \n");
	printf("     | -   |   - |  \n");
	printf("     o-----o-----o  \n");
    }
	
    // Simulation properties
    struct solverInfo info = readBasicInfo( pid, world );
    
    // Read Fields
    struct twoPhasesFields fields;

    // Neighbours indices
    fields.nb = readNeighbours(&info.lattice, &info.parallel);
    if(pid == 0) { printf("\nReading neighbour indices\n"); }
    
    // Order parameter
    fields.phi     = readScalarField("phi", &info.lattice, &info.parallel, &info.time);
    fields.phi_old = readScalarField("phi", &info.lattice, &info.parallel, &info.time);
    if(pid == 0) { printf("\nReading field phi\n"); }

    // Chemical potential
    fields.muPhi = readScalarField("muPhi", &info.lattice, &info.parallel, &info.time);
    if(pid == 0) { printf("\nReading field muPhi\n"); }

    // Pressure
    fields.p = readScalarField("p", &info.lattice, &info.parallel, &info.time);
    if(pid == 0) { printf("\nReading field p\n"); }

    // Density
    fields.rho = readScalarField("rho", &info.lattice, &info.parallel, &info.time);
    if(pid == 0) { printf("\nReading field rho\n");  }

    // Velocity
    fields.U     = readVectorField("U", &info.lattice, &info.parallel, &info.time);
    fields.U_old = readVectorField("U", &info.lattice, &info.parallel, &info.time);
    if(pid == 0) { printf("\nReading field U\n");  }

    // Cahn-Hilliard field
    fields.h = readPdfField("h", &info.lattice, &info.parallel, &info.time);
    if(pid == 0) { printf("\nReading field h\n");  }

    // Navier-Stokes field
    fields.g = readPdfField("g", &info.lattice, &info.parallel, &info.time);
    fields.swp = readPdfField("g", &info.lattice, &info.parallel, &info.time);
    if(pid == 0) { printf("\nReading field g\n\n\n");  }
    


    // Synchronize initial fields
    syncScalarField(&info.parallel, fields.phi );
    syncScalarField(&info.parallel, fields.muPhi );
    syncScalarField(&info.parallel, fields.p );
    syncScalarField(&info.parallel, fields.rho );

    syncPdfField(&info.parallel, fields.U, 3 );
    syncPdfField(&info.parallel, fields.h, info.lattice.Q );
    syncPdfField(&info.parallel, fields.g, info.lattice.Q );


    
    // Advance in time. Collide, stream, update and write
    while( updateTime(&info.time) ) {


	// Collide h (Cahn-Hilliard)
	cahnHilliardCollision(&fields, &info);


	
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
	

	
	
	// Collide g (Navier - Stokes)
	liangCollision(&fields, &info);

	
	/* // Stream */
	/* lbstream( fields.h, fields.swp, fields.nb, &info.lattice, &info.parallel ); */
	/* lbstream( fields.g, fields.swp, fields.nb, &info.lattice, &info.parallel ); */



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
	
	
	// Update macroscopic fields

	// Order parameter
	orderParameter( &fields, &info, fields.phi );
	
	// Chemical potential
	chemicalPotential( &fields, &info, fields.muPhi );

	// Velocity
	velocity( &fields, &info, fields.U );

	// Pressure
	pressureWithU( &fields, &info, fields.p );
	
	// Density
	density( &fields, &info, fields.rho );



	
	
    	// Write fields
    	if( writeFlag(&info.time) ) {
	    
	    if(pid == 0) {
		printf("Time = %.2f (%d)\n", (double)info.time.current * info.time.tstep, info.time.current);
		printf("Elapsed time = %.2f seconds\n\n", elapsed(&info.time) );
	    }
	    
    	    // ScalarFields
    	    writeScalarField("phi", fields.phi, &info.lattice, &info.parallel, &info.time);
    	    writeScalarField("muPhi", fields.muPhi, &info.lattice, &info.parallel, &info.time);
    	    writeScalarField("rho", fields.rho, &info.lattice, &info.parallel, &info.time);
    	    writeScalarField("p", fields.p, &info.lattice, &info.parallel, &info.time);

    	    // Vector fields
    	    writeVectorField("U", fields.U, &info.lattice, &info.parallel, &info.time);

    	    // Pdf fields
    	    writePdfField("h", fields.h, &info.lattice, &info.parallel, &info.time);
    	    writePdfField("g", fields.g, &info.lattice, &info.parallel, &info.time);
	    
    	}

    }


    
    // Print info
    if(pid == 0) {
	printf("\n  Finished in %.2f seconds \n\n", elapsed(&info.time) );
    }


    MPI_Finalize();
    
    return 0;
    
}
