/* 
** creategrid.c
**
** written by Marcel Zemp
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <assert.h>
#include <math.h>
#include <rpc/types.h>
#include <rpc/xdr.h>
#include <iof.h>

void usage(void);

int main(int argc, char **argv) {

	int i,j,k;
	int verboselevel;
	int NBox, Ntot;
	double LCell;
	TIPSY_HEADER th;
	TIPSY_DARK_PARTICLE dp;
	XDR xdrs;

	verboselevel = 0;
	LCell = 1;
	NBox = 256;

	i = 1;
	while (i < argc) {
		if (strcmp(argv[i],"-LCell") == 0) {
			i++;
			if (i >= argc) {
				usage();
				}
			LCell = atof(argv[i]);
			i++;
			}
		else if (strcmp(argv[i],"-NBox") == 0) {
			i++;
			if (i >= argc) {
				usage();
				}
			NBox = atoi(argv[i]);
			i++;
			}
		else if (strcmp(argv[i],"-v") == 0) {
			verboselevel = 1;
			i++;
			}
		else if ((strcmp(argv[i],"-h") == 0) || (strcmp(argv[i],"-help") == 0)) {
			usage();
			}
		else {
			usage();
			}
		}

	Ntot = NBox*NBox*NBox;

	th.time = 0;
	th.ntotal = Ntot;
	th.ndim = 3;
	th.ngas = 0;
	th.ndark = Ntot;
	th.nstar = 0;

	xdrstdio_create(&xdrs,stdout,XDR_ENCODE);
	write_tipsy_xdr_header(&xdrs,&th);

	/*
	** Go through particles and find gaps in high-resolution region
	*/

	for (k = 0; k < NBox; k++) {
		for (j = 0; j < NBox; j++) {
			for (i = 0; i < NBox; i++) {
				dp.pos[0] = LCell/2.0 + i*LCell;
				dp.pos[1] = LCell/2.0 + j*LCell;
				dp.pos[2] = LCell/2.0 + k*LCell;
				dp.mass = 1;
				dp.eps = 1;
				dp.phi = 1;
				write_tipsy_xdr_dark(&xdrs,&dp);
				}
			}
		}
	xdr_destroy(&xdrs);

	/*
	** Give some additonal output
	*/

	if (verboselevel >= 1) {
		fprintf(stderr,"Used values:\n");
		fprintf(stderr,"LCell : %.6e\n",LCell);
		fprintf(stderr,"NBox  : %d\n",NBox);
		}

	exit(0);
	}

void usage(void) {

	fprintf(stderr,"\n");
	fprintf(stderr,"Program writes out perfect grid of dark matter particles.\n");
	fprintf(stderr,"\n");
	fprintf(stderr,"Please specify the following parameters:\n");
	fprintf(stderr,"\n");
	fprintf(stderr,"-LCell	 : Length of basic cell (default: 1)\n");
	fprintf(stderr,"-NBox	 : Number of particles per box length (default: 256)\n");
	fprintf(stderr,"-v		 : get a bit more details\n");
	fprintf(stderr,"> <name> : output file in tipsy standard binary format\n");
	fprintf(stderr,"\n");
	exit(1);
	}
