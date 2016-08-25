#include <density.h>
#include <stdio.h>

void density( struct twoPhasesFields* fields, const struct solverInfo* info, double* fld ) {

    unsigned int id;

    // Move over points
    for( id = 0 ; id < info->lattice.nlocal ; id++ ) {

	fld[id] = (fields->phi[id] - info->fields.phi_B) * (info->fields.rho_A - info->fields.rho_B) / (info->fields.phi_A - info->fields.phi_B) + info->fields.rho_B;

    }
    
}
