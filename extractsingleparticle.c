/* 
** extractsingleparticle.c
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
    int index, indextot;
    int positionprecision;
    TIPSY_HEADER th;
    GAS_PARTICLE gp;
    DARK_PARTICLE dp;
    STAR_PARTICLE sp;
    GAS_PARTICLE_DPP gpdpp;
    DARK_PARTICLE_DPP dpdpp;
    STAR_PARTICLE_DPP spdpp;
    XDR xdrs;

    index = -1;
    positionprecision = 0;
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
	else if (strcmp(argv[i],"-index") == 0) {
	    i++;
	    if (i >= argc) {
		usage();
		}
	    index = atoi(argv[i])-1;
	    i++;
	    }
	else if ((strcmp(argv[i],"-h") == 0) || (strcmp(argv[i],"-help") == 0)) {
	    usage();
	    }
	else {
	    usage();
	    }
	}
    assert(index >= 0);
    /*
    ** Read & write particles
    */
    xdrstdio_create(&xdrs,stdin,XDR_DECODE);
    read_tipsy_standard_header(&xdrs,&th);
    assert(index < th.ntotal);
    if (positionprecision == 0) {
	for (i = 0; i < th.ngas; i++) {
	    read_tipsy_standard_gas(&xdrs,&gp);
	    indextot = i;
	    if (index == indextot) {
		fprintf(stdout,"r = (%+.6e, %+.6e, %+.6e) v = (%+.6e, %+.6e, %+.6e) m = %.6e eps = %.6e ID = %d  type: gas\n",
			gp.pos[0],gp.pos[1],gp.pos[2],gp.vel[0],gp.vel[1],gp.vel[2],gp.mass,gp.hsmooth,indextot+1);
		exit(0);
		}
	    }
	for (i = 0; i < th.ndark; i++) {
	    read_tipsy_standard_dark(&xdrs,&dp);
	    indextot = th.ngas+i;
	    if (index == indextot) {
		fprintf(stdout,"r = (%+.6e, %+.6e, %+.6e) v = (%+.6e, %+.6e, %+.6e) m = %.6e eps = %.6e ID = %d type: dark\n",
			dp.pos[0],dp.pos[1],dp.pos[2],dp.vel[0],dp.vel[1],dp.vel[2],dp.mass,dp.eps,indextot+1);
		exit(0);
		}
	    }
	for (i = 0; i < th.nstar; i++) {
	    read_tipsy_standard_star(&xdrs,&sp);
	    indextot = th.ngas+th.ndark+i;
	    if (index == indextot) {
		fprintf(stdout,"r = (%+.6e, %+.6e, %+.6e) v = (%+.6e, %+.6e, %+.6e) m = %.6e eps = %.6e ID = %d type: star\n",
			sp.pos[0],sp.pos[1],sp.pos[2],sp.vel[0],sp.vel[1],sp.vel[2],sp.mass,sp.eps,indextot+1);
		exit(0);
		}
	    }
	}
    else if (positionprecision == 1) {
	for (i = 0; i < th.ngas; i++) {
	    read_tipsy_standard_gas_dpp(&xdrs,&gpdpp);
	    }
	for (i = 0; i < th.ndark; i++) {
	    read_tipsy_standard_dark_dpp(&xdrs,&dpdpp);
	    }
	for (i = 0; i < th.nstar; i++) {
	    read_tipsy_standard_star_dpp(&xdrs,&spdpp);
	    }
	}
    exit(0);
    }

void usage(void) {

    fprintf(stderr,"\n");
    fprintf(stderr,"Program extracts particles with index i and writes some info to stdout.\n\n");
    fprintf(stderr,"You can specify the following arguments:\n\n");
    fprintf(stderr,"-spp             : set this flag if input and output file have single precision positions (default)\n");
    fprintf(stderr,"-dpp             : set this flag if input and output file have double precision positions\n");
    fprintf(stderr,"-index <type><n> : <type>: i (int), f (float) or d (double), <n> array index\n");
    fprintf(stderr,"< <name>         : input file in tipsy standard binary format\n");
    fprintf(stderr,"\n");
    exit(1);
    }
