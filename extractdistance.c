/* 
** extractdistance.c
**
** Written by Marcel Zemp
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <assert.h>
#include <math.h>
#include <iof.h>

void usage(void);

int main(int argc, char **argv) {

	int i;
	int positionprecision, verboselevel;
	double r;
	TIPSY_HEADER th;
	TIPSY_GAS_PARTICLE gp;
	TIPSY_DARK_PARTICLE dp;
	TIPSY_STAR_PARTICLE sp;
	TIPSY_GAS_PARTICLE_DPP gpdpp;
	TIPSY_DARK_PARTICLE_DPP dpdpp;
	TIPSY_STAR_PARTICLE_DPP spdpp;
	XDR xdrs;

	positionprecision = 0;
	verboselevel = 0;
	i = 1;
	while (i < argc) {
		if ((strcmp(argv[i],"-h") == 0) || (strcmp(argv[i],"-help") == 0)) {
			usage();
			}
		if (strcmp(argv[i],"-version") == 0) {
			fprintf(stderr,"%s (%s)\n",NAME,VERSION);
			exit(1);
			}
		if (strcmp(argv[i],"-spp") == 0) {
			positionprecision = 0;
			i++;
			}
		else if (strcmp(argv[i],"-dpp") == 0) {
			positionprecision = 1;
			i++;
			}
		else if (strcmp(argv[i],"-verbose") == 0) {
			verboselevel = 1;
			i++;
			}
		else {
			usage();
			}
		}
	xdrstdio_create(&xdrs,stdin,XDR_DECODE);
	read_tipsy_xdr_header(&xdrs,&th);
	fprintf(stdout,"%d\n",th.ntotal);
	if (positionprecision == 0) {
		for (i = 0; i < th.ngas; i++) {
			read_tipsy_xdr_gas(&xdrs,&gp);
			r = sqrt(gp.pos[0]*gp.pos[0] + gp.pos[1]*gp.pos[1] + gp.pos[2]*gp.pos[2]);
			assert(fprintf(stdout,"%.6e\n",r) > 0);
			}
		for (i = 0; i < th.ndark; i++) {
			read_tipsy_xdr_dark(&xdrs,&dp);
			r = sqrt(dp.pos[0]*dp.pos[0] + dp.pos[1]*dp.pos[1] + dp.pos[2]*dp.pos[2]);
			assert(fprintf(stdout,"%.6e\n",r) > 0);
			}
		for (i = 0; i < th.nstar; i++) {
			read_tipsy_xdr_star(&xdrs,&sp);
			r = sqrt(sp.pos[0]*sp.pos[0] + sp.pos[1]*sp.pos[1] + sp.pos[2]*sp.pos[2]);
			assert(fprintf(stdout,"%.6e\n",r) > 0);
			}
		}
	if (positionprecision == 1) {
		for (i = 0; i < th.ngas; i++) {
			read_tipsy_xdr_gas_dpp(&xdrs,&gpdpp);
			r = sqrt(gpdpp.pos[0]*gpdpp.pos[0] + gpdpp.pos[1]*gpdpp.pos[1] + gpdpp.pos[2]*gpdpp.pos[2]);
			assert(fprintf(stdout,"%.6e\n",r) > 0);
			}
		for (i = 0; i < th.ndark; i++) {
			read_tipsy_xdr_dark_dpp(&xdrs,&dpdpp);
			r = sqrt(dpdpp.pos[0]*dpdpp.pos[0] + dpdpp.pos[1]*dpdpp.pos[1] + dpdpp.pos[2]*dpdpp.pos[2]);
			assert(fprintf(stdout,"%.6e\n",r) > 0);
			}
		for (i = 0; i < th.nstar; i++) {
			read_tipsy_xdr_star_dpp(&xdrs,&spdpp);
			r = sqrt(spdpp.pos[0]*spdpp.pos[0] + spdpp.pos[1]*spdpp.pos[1] + spdpp.pos[2]*spdpp.pos[2]);
			assert(fprintf(stdout,"%.6e\n",r) > 0);
			}
		}
	xdr_destroy(&xdrs);
	if (verboselevel > 0) {
		fprintf(stderr,"Time: %g Ntotal: %d Ngas: %d Ndark: %d Nstar: %d\n",
			th.time,th.ntotal,th.ngas,th.ndark,th.nstar);
		}
	exit(0);
	}

void usage(void) {

	fprintf(stderr,"\n");
	fprintf(stderr,"%s (%s)\n",NAME,VERSION);
	fprintf(stderr,"\n");
	fprintf(stderr,"Program extracts the distance with respect to the origin of the particles from\n");
	fprintf(stderr,"the input file and writes an tipsy ascii array.\n");
	fprintf(stderr,"\n");
	fprintf(stderr,"Please specify the following parameters:\n");
	fprintf(stderr,"\n");
	fprintf(stderr,"-spp     : set this flag if input and output files have single precision positions (default)\n");
	fprintf(stderr,"-dpp     : set this flag if input and output files have double precision positions\n");
	fprintf(stderr,"< <name> : input file in tipsy standard binary format\n");
	fprintf(stderr,"> <name> : output file in tipsy ascii array format\n");
	fprintf(stderr,"\n");
	fprintf(stderr,"Other options:\n");
	fprintf(stderr,"\n");
	fprintf(stderr,"-h or -help : display this help and exit\n");
	fprintf(stderr,"-version    : display version information and exit\n");
	fprintf(stderr,"\n");
	exit(1);
	}
