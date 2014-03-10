/* 
** scaleas.c
**
** written by Marcel Zemp
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <assert.h>
#include <iof.h>

void usage(void);

int main(int argc, char **argv) {

	int i;
	int shiftint, factorint;
	int integerindex, floatindex, doubleindex, index;
	double shift, factor;
	ARRAY_HEADER ah;
	ARRAY_PARTICLE ap;
	XDR xdrsin, xdrsout;

	index = -1;
	integerindex = 0;
	floatindex = 0;
	doubleindex = 0;
	shiftint = 0;
	factorint = 1;
	shift = 0;
	factor = 1;
	i = 1;
	while (i < argc) {
		if (strcmp(argv[i],"-shift") == 0) {
			i++;
			if (i >= argc) {
				usage();
				}
			shift = atof(argv[i]);
			i++;
			}
		else if (strcmp(argv[i],"-factor") == 0) {
			i++;
			if (i >= argc) {
				usage();
				}
			factor = atof(argv[i]);
			i++;
			}
		else if (strcmp(argv[i],"-index") == 0) {
			i++;
			if (i >= argc) {
				usage();
				}
			if (strcmp(argv[i],"i") == 0) {
				integerindex = 1;
				}
			else if (strcmp(argv[i],"f") == 0) {
				floatindex = 1;
				}
			else if (strcmp(argv[i],"d") == 0) {
				doubleindex = 1;
				}
			else {
				usage();
				}
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
	assert(index != -1);
	if (integerindex == 1) {
		shiftint = (int) shift;
		factorint = (int) factor;
		}
	xdrstdio_create(&xdrsin,stdin,XDR_DECODE);
	xdrstdio_create(&xdrsout,stdout,XDR_ENCODE);
	read_array_xdr_header(&xdrsin,&ah);
	allocate_array_particle(&ah,&ap);
	write_array_xdr_header(&xdrsout,&ah);
	for (i = 0; i < ah.N[0]; i++) {
		read_array_xdr_particle(&xdrsin,&ah,&ap);
		if (integerindex == 1) {
			ap.ia[index] = factorint*(ap.ia[index]+shiftint);
			}
		else if (floatindex == 1) {
			ap.fa[index] = factor*(ap.fa[index]+shift);
			}
		else if (doubleindex == 1) {
			ap.da[index] = factor*(ap.da[index]+shift);
			}
		write_array_xdr_particle(&xdrsout,&ah,&ap);
		}
	xdr_destroy(&xdrsin);
	xdr_destroy(&xdrsout);
	fprintf(stderr,"Ntotal: %d Ni: %d Nf: %d Nd: %d\n",
		ah.N[0],ah.N[1],ah.N[2],ah.N[3]);
	if (integerindex == 1) {
		fprintf(stderr,"shift: %d factor: %d\n",shiftint,factorint);
		}
	else {
		fprintf(stderr,"shift: %g factor: %g\n",shift,factor);
		}
	exit(0);
	}

void usage(void) {

	fprintf(stderr,"\n");
	fprintf(stderr,"Program performes the following scaling on the array values v_old:\n");
	fprintf(stderr,"v_new = factor * (v_old + shift)\n");
	fprintf(stderr,"\n");
	fprintf(stderr,"Please specify the following parameters:\n");
	fprintf(stderr,"\n");
	fprintf(stderr,"-shift <value>        : value for shift\n");
	fprintf(stderr,"-factor <value>       : value for factor\n");
	fprintf(stderr,"-index <type> <index> : <type>: i, f or d, index of array field\n");
	fprintf(stderr,"< <name>              : input file in standard binary format\n");
	fprintf(stderr,"> <name>              : output file in standard binary format\n");
	fprintf(stderr,"\n");
	exit(1);
	}
