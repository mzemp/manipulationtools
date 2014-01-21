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
	TIPSY_GAS_PARTICLE gp;
	TIPSY_DARK_PARTICLE dp;
	TIPSY_STAR_PARTICLE sp;
	TIPSY_GAS_PARTICLE_DPP gpdpp;
	TIPSY_DARK_PARTICLE_DPP dpdpp;
	TIPSY_STAR_PARTICLE_DPP spdpp;
	XDR xdrs;

	index = -1;
	indextot = 0;
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
	read_tipsy_xdr_header(&xdrs,&th);
	assert(index < th.ntotal);
	if (positionprecision == 0) {
		for (i = 0; i < th.ngas; i++) {
			read_tipsy_xdr_gas(&xdrs,&gp);
			indextot = i;
			if (index == indextot) {
				fprintf(stdout,"r = (%+.6e, %+.6e, %+.6e) v = (%+.6e, %+.6e, %+.6e) m = %.6e eps = %.6e ID = %d	 type: gas\n",
					gp.pos[0],gp.pos[1],gp.pos[2],gp.vel[0],gp.vel[1],gp.vel[2],gp.mass,gp.hsmooth,indextot+1);
				exit(0);
				}
			}
		for (i = 0; i < th.ndark; i++) {
			read_tipsy_xdr_dark(&xdrs,&dp);
			indextot = th.ngas+i;
			if (index == indextot) {
				fprintf(stdout,"r = (%+.6e, %+.6e, %+.6e) v = (%+.6e, %+.6e, %+.6e) m = %.6e eps = %.6e ID = %d type: dark\n",
					dp.pos[0],dp.pos[1],dp.pos[2],dp.vel[0],dp.vel[1],dp.vel[2],dp.mass,dp.eps,indextot+1);
				exit(0);
				}
			}
		for (i = 0; i < th.nstar; i++) {
			read_tipsy_xdr_star(&xdrs,&sp);
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
			read_tipsy_xdr_gas_dpp(&xdrs,&gpdpp);
			indextot = i;
			if (index == indextot) {
				fprintf(stdout,"r = (%+.14e, %+.14e, %+.14e) v = (%+.6e, %+.6e, %+.6e) m = %.6e eps = %.6e ID = %d	type: gas\n",
					gpdpp.pos[0],gpdpp.pos[1],gpdpp.pos[2],gpdpp.vel[0],gpdpp.vel[1],gpdpp.vel[2],gpdpp.mass,gpdpp.hsmooth,indextot+1);
				exit(0);
				}
			}
		for (i = 0; i < th.ndark; i++) {
			read_tipsy_xdr_dark_dpp(&xdrs,&dpdpp);
			indextot = th.ngas+i;
			if (index == indextot) {
				fprintf(stdout,"r = (%+.14e, %+.14e, %+.14e) v = (%+.6e, %+.6e, %+.6e) m = %.6e eps = %.6e ID = %d type: dark\n",
					dpdpp.pos[0],dpdpp.pos[1],dpdpp.pos[2],dpdpp.vel[0],dpdpp.vel[1],dpdpp.vel[2],dpdpp.mass,dpdpp.eps,indextot+1);
				exit(0);
				}
			}
		for (i = 0; i < th.nstar; i++) {
			read_tipsy_xdr_star_dpp(&xdrs,&spdpp);
			if (index == indextot) {
				fprintf(stdout,"r = (%+.14e, %+.14e, %+.14e) v = (%+.6e, %+.6e, %+.6e) m = %.6e eps = %.6e ID = %d type: star\n",
					spdpp.pos[0],spdpp.pos[1],spdpp.pos[2],spdpp.vel[0],spdpp.vel[1],spdpp.vel[2],spdpp.mass,spdpp.eps,indextot+1);
				exit(0);
				}
			}
		}
	exit(0);
	}

void usage(void) {

	fprintf(stderr,"\n");
	fprintf(stderr,"Program extracts particles with index i and writes some info to stdout.\n\n");
	fprintf(stderr,"You can specify the following arguments:\n\n");
	fprintf(stderr,"-spp	   : set this flag if input and output file have single precision positions (default)\n");
	fprintf(stderr,"-dpp	   : set this flag if input and output file have double precision positions\n");
	fprintf(stderr,"-index <i> : particle index\n");
	fprintf(stderr,"< <name>   : input file in tipsy standard binary format\n");
	fprintf(stderr,"\n");
	exit(1);
	}
