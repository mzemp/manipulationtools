/* 
** extract.c
**
** written by Marcel Zemp
*/

#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <assert.h>
#include <rpc/types.h>
#include <rpc/xdr.h>
#include <iof.h>

void usage(void);

int main(int argc, char **argv) {

    int i;
    int shift, delta;
    int positionprecision;
    TIPSY_HEADER th;
    GAS_PARTICLE gp;
    DARK_PARTICLE dp;
    STAR_PARTICLE sp;
    GAS_PARTICLE_DPP gpdpp;
    DARK_PARTICLE_DPP dpdpp;
    STAR_PARTICLE_DPP spdpp;
/*     ARRAY_HEADER ah; */
/*     ARRAY_PARTICLE ap; */
    XDR xdrsin;

    positionprecision = 0;
    shift = 0;
    delta = 1;
    /*
    ** Read in arguments
    */
    i = 1;
    while (i < argc) {
        if (strcmp(argv[i],"-spp") == 0) {
            positionprecision = 0;
            i++;
            }
        else if (strcmp(argv[i],"-dpp") == 0) {
            positionprecision = 1;
            i++;
            }
	else if (strcmp(argv[i],"-shift") == 0) {
	    i++;
	    if (i >= argc) {
		usage();
		}
	    shift = atoi(argv[i]);
	    i++;
	    }
	else if (strcmp(argv[i],"-delta") == 0) {
	    i++;
	    if (i >= argc) {
		usage();
		}
	    delta = atoi(argv[i]);
	    i++;
	    }
	else if ((strcmp(argv[i],"-h") == 0) || (strcmp(argv[i],"-help") == 0)) {
	    usage();
	    }
	else {
	    usage();
	    }
	}
    /*
    ** Read & write particles
    */
    xdrstdio_create(&xdrsin,stdin,XDR_DECODE);
    read_tipsy_standard_header(&xdrsin,&th);
    if (positionprecision == 0) {
	for (i = 0; i < th.ngas; i++) {
	    read_tipsy_standard_gas(&xdrsin,&gp);
	    if ((i+1-shift)%delta == 0) {
		fprintf(stdout,"%+.6e %+.6e %+.6e %+.6e %+.6e %+.6e ",gp.pos[0],gp.pos[1],gp.pos[2],gp.vel[0],gp.vel[1],gp.vel[2]);
		fprintf(stdout,"\n");
		}
	    }
	for (i = 0; i < th.ndark; i++) {
	    read_tipsy_standard_dark(&xdrsin,&dp);
	    if ((i+1-shift)%delta == 0) {
		fprintf(stdout,"%+.6e %+.6e %+.6e %+.6e %+.6e %+.6e ",dp.pos[0],dp.pos[1],dp.pos[2],dp.vel[0],dp.vel[1],dp.vel[2]);
		fprintf(stdout,"\n");
		}
	    }
	for (i = 0; i < th.nstar; i++) {
	    read_tipsy_standard_star(&xdrsin,&sp);
	    if ((i+1-shift)%delta == 0) {
		fprintf(stdout,"%+.6e %+.6e %+.6e %+.6e %+.6e %+.6e ",sp.pos[0],sp.pos[1],sp.pos[2],sp.vel[0],sp.vel[1],sp.vel[2]);
		fprintf(stdout,"\n");
		}
	    }
	}
    else if (positionprecision == 1) {
	for (i = 0; i < th.ngas; i++) {
	    read_tipsy_standard_gas_dpp(&xdrsin,&gpdpp);
	    if ((i+1-shift)%delta == 0) {
		fprintf(stdout,"%+.14e %+.14e %+.14e %+.6e %+.6e %+.6e ",gpdpp.pos[0],gpdpp.pos[1],gpdpp.pos[2],gpdpp.vel[0],gpdpp.vel[1],gpdpp.vel[2]);
		fprintf(stdout,"\n");
		}
	    }
	for (i = 0; i < th.ndark; i++) {
	    read_tipsy_standard_dark_dpp(&xdrsin,&dpdpp);
	    if ((i+1-shift)%delta == 0) {
		fprintf(stdout,"%+.14e %+.14e %+.14e %+.6e %+.6e %+.6e ",dpdpp.pos[0],dpdpp.pos[1],dpdpp.pos[2],dpdpp.vel[0],dpdpp.vel[1],dpdpp.vel[2]);
		fprintf(stdout,"\n");
		}
	    }
	for (i = 0; i < th.nstar; i++) {
	    read_tipsy_standard_star_dpp(&xdrsin,&spdpp);
	    if ((i+1-shift)%delta == 0) {
		fprintf(stdout,"%+.14e %+.14e %+.14e %+.6e %+.6e %+.6e ",spdpp.pos[0],spdpp.pos[1],spdpp.pos[2],spdpp.vel[0],spdpp.vel[1],spdpp.vel[2]);
		fprintf(stdout,"\n");
		}
	    }
	}
    xdr_destroy(&xdrsin);
    exit(0);
    }

void usage(void) {

    fprintf(stderr,"\n");
    fprintf(stderr,"Program extracts particles if the index i staisfies (i+Shift) mod Delta == 0.\n\n");
    fprintf(stderr,"You can specify the following arguments:\n\n");
    fprintf(stderr,"-spp           : set this flag if input and output file have single precision positions (default)\n");
    fprintf(stderr,"-dpp           : set this flag if input and output file have double precision positions\n");
    fprintf(stderr,"-shift <value> : index shift (default: 0)\n");
    fprintf(stderr,"-delta <value> : index delta (default: 1)\n");
    fprintf(stderr,"< <name>       : input file in tipsy standard binary format\n");
    fprintf(stderr,"> <name>       : output file in text format\n");
    exit(1);
    }
