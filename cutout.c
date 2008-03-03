/* 
** cutout.c
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

    int i, j;
    int positionprecision;
    double xcen[3], dx[3], dxi[3], dxo[3], r, ri, ro;
    char outname[100], tempname1[100], tempname2[100];
    TIPSY_HEADER thin, thout;
    GAS_PARTICLE gp;
    DARK_PARTICLE dp;
    STAR_PARTICLE sp;
    GAS_PARTICLE_DPP gpdpp;
    DARK_PARTICLE_DPP dpdpp;
    STAR_PARTICLE_DPP spdpp;
    ARRAY_HEADER ahin, ahout;
    ARRAY_PARTICLE ap;
    FILE *fin1, *fin2, *fout1, *fout2;
    XDR xdrsin1, xdrsin2, xdrsout1, xdrsout2;

    /*
    ** Set default values
    */
    positionprecision = 0;
    for (j = 0; j < 3; j++) {
	xcen[j] = 0;
	dx[j] = 0;
	dxi[j] = 0;
	dxo[j] = 0;
	}
    r = 0;
    ri = 0;
    ro = 0;
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
	else if (strcmp(argv[i],"-rxcen") == 0) {
	    i++;
	    if (i >= argc) {
		usage();
		}
	    xcen[0] = atof(argv[i]);
	    i++;
	    }
	else if (strcmp(argv[i],"-rycen") == 0) {
	    i++;
	    if (i >= argc) {
		usage();
		}
	    xcen[1] = atof(argv[i]);
	    i++;
	    }
	else if (strcmp(argv[i],"-rzcen") == 0) {
	    i++;
	    if (i >= argc) {
		usage();
		}
	    xcen[2] = atof(argv[i]);
	    i++;
	    }
	else if (strcmp(argv[i],"-drxi") == 0) {
	    i++;
	    if (i >= argc) {
		usage();
		}
	    dxi[0] = atof(argv[i]);
	    i++;
	    }
	else if (strcmp(argv[i],"-dryi") == 0) {
	    i++;
	    if (i >= argc) {
		usage();
		}
	    dxi[1] = atof(argv[i]);
	    i++;
	    }
	else if (strcmp(argv[i],"-drzi") == 0) {
	    i++;
	    if (i >= argc) {
		usage();
		}
	    dxi[2] = atof(argv[i]);
	    i++;
	    }
	else if (strcmp(argv[i],"-dxo") == 0) {
	    i++;
	    if (i >= argc) {
		usage();
		}
	    dxo[0] = atof(argv[i]);
	    i++;
	    }
	else if (strcmp(argv[i],"-dryo") == 0) {
	    i++;
	    if (i >= argc) {
		usage();
		}
	    dxo[1] = atof(argv[i]);
	    i++;
	    }
	else if (strcmp(argv[i],"-drzo") == 0) {
	    i++;
	    if (i >= argc) {
		usage();
		}
	    dxo[2] = atof(argv[i]);
	    i++;
	    }
	else if (strcmp(argv[i],"-ri") == 0) {
	    i++;
	    if (i >= argc) {
		usage();
		}
	    ri = atof(argv[i]);
	    i++;
	    }
	else if (strcmp(argv[i],"-ro") == 0) {
	    i++;
	    if (i >= argc) {
		usage();
		}
	    ro = atof(argv[i]);
	    i++;
	    }
        else if (strcmp(argv[i],"-o") == 0) {
            i++;
            if (i >= argc) {
                usage();
                }
	    strcpy(outname,argv[i]);
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
    ** Check some things
    */
    if ((ri == 0) && (ro == 0)) {
	for (j = 0; j < 3; j++) {
	    assert(dxo[j] != 0);
	    }
	}
    else {
	for (j = 0; j < 3; j++) {
	    assert(dxi[j] == 0);
	    assert(dxo[j] == 0);
	    }
	}
    /*
    ** Read in header
    */
    xdrstdio_create(&xdrsin1,stdin,XDR_DECODE);
    read_tipsy_standard_header(&xdrsin1,&thin);
    /*
    ** Write out temporary file to be replaced later
    */
    sprintf(tempname1,"tf1_%s",outname);
    fout1 = fopen(tempname1,"w");
    assert(fout1 != NULL);
    xdrstdio_create(&xdrsout1,fout1,XDR_ENCODE);
    thout.time = thin.time;
    thout.ntotal = 0;
    thout.ndim = thin.ndim;
    thout.ngas = 0;
    thout.ndark = 0;
    thout.nstar = 0;
    write_tipsy_standard_header(&xdrsout1,&thin);
    sprintf(tempname2,"tf2_%s",outname);
    fout2 = fopen(tempname2,"w");
    assert(fout2 != NULL);
    xdrstdio_create(&xdrsout2,fout2,XDR_ENCODE);
    ahout.N[0] = 0;
    ahout.N[1] = 1;
    ahout.N[2] = 0;
    ahout.N[3] = 0;
    allocate_array_particle(&ahout,&ap);
    write_array_header(&xdrsout2,&ahout);
    /*
    ** Check if particles are written out
    */
    if ((ri == 0) && (ro == 0)) {
	for (i = 0; i < thin.ngas; i++) {
	    if (positionprecision == 0) {
		read_tipsy_standard_gas(&xdrsin1,&gp);
		for (j = 0; j < 3; j++) {
		    dx[j] = fabs(gp.pos[j]-xcen[j]);
		    }
		}
	    else if (positionprecision == 1) {
		read_tipsy_standard_gas_dpp(&xdrsin1,&gpdpp);
		for (j = 0; j < 3; j++) {
		    dx[j] = fabs(gpdpp.pos[j]-xcen[j]);
		    }
		}
	    if (((dx[0] < dxo[0]/2.0) && (dx[1] < dxo[1]/2.0) && (dx[2] < dxo[2]/2.0)) &&
		!((dx[0] < dxi[0]/2.0) && (dx[1] < dxi[1]/2.0) && (dx[2] < dxi[2]/2.0))) {
		thout.ngas++;
		if (positionprecision == 0) {
		    write_tipsy_standard_gas(&xdrsout1,&gp);
		    }
		else if (positionprecision == 1) {
		    write_tipsy_standard_gas_dpp(&xdrsout1,&gpdpp);
		    }
		ap.ia[0] = 1+i;
		write_array_particle(&xdrsout2,&ahout,&ap);
		}
	    }
	for (i = 0; i < thin.ndark; i++) {
	    if (positionprecision == 0) {
		read_tipsy_standard_dark(&xdrsin1,&dp);
		for (j = 0; j < 3; j++) {
		    dx[j] = fabs(dp.pos[j]-xcen[j]);
		    }
		}
	    else if (positionprecision == 1) {
		read_tipsy_standard_dark_dpp(&xdrsin1,&dpdpp);
		for (j = 0; j < 3; j++) {
		    dx[j] = fabs(dpdpp.pos[j]-xcen[j]);
		    }
		}
	    if (((dx[0] < dxo[0]/2.0) && (dx[1] < dxo[1]/2.0) && (dx[2] < dxo[2]/2.0)) &&
		!((dx[0] < dxi[0]/2.0) && (dx[1] < dxi[1]/2.0) && (dx[2] < dxi[2]/2.0))) {
		thout.ndark++;
		if (positionprecision == 0) {
		    write_tipsy_standard_dark(&xdrsout1,&dp);
		    }
		else if (positionprecision == 1) {
		    write_tipsy_standard_dark_dpp(&xdrsout1,&dpdpp);
		    }
		ap.ia[0] = thin.ngas+1+i;
		write_array_particle(&xdrsout2,&ahout,&ap);
		}
	    }
	for (i = 0; i < thin.nstar; i++) {
	    if (positionprecision == 0) {
		read_tipsy_standard_star(&xdrsin1,&sp);
		for (j = 0; j < 3; j++) {
		    dx[j] = fabs(sp.pos[j]-xcen[j]);
		    }
		}
	    else if (positionprecision == 1) {
		read_tipsy_standard_star_dpp(&xdrsin1,&spdpp);
		for (j = 0; j < 3; j++) {
		    dx[j] = fabs(spdpp.pos[j]-xcen[j]);
		    }
		}
	    if (((dx[0] < dxo[0]/2.0) && (dx[1] < dxo[1]/2.0) && (dx[2] < dxo[2]/2.0)) &&
		!((dx[0] < dxi[0]/2.0) && (dx[1] < dxi[1]/2.0) && (dx[2] < dxi[2]/2.0))) {
		thout.nstar++;
		if (positionprecision == 0) {
		    write_tipsy_standard_star(&xdrsout1,&sp);
		    }
		else if (positionprecision == 1) {
		    write_tipsy_standard_star_dpp(&xdrsout1,&spdpp);
		    }
		ap.ia[0] = thin.ngas+thin.ndark+1+i;
		write_array_particle(&xdrsout2,&ahout,&ap);
		}
	    }
	}
    else {
	for (i = 0; i < thin.ngas; i++) {
	    r = 0;
	    if (positionprecision == 0) {
		read_tipsy_standard_gas(&xdrsin1,&gp);
		for (j = 0; j < 3; j++) {
		    r += (gp.pos[j]-xcen[j])*(gp.pos[j]-xcen[j]);
		    }
		}
	    else if (positionprecision == 1) {
		read_tipsy_standard_gas_dpp(&xdrsin1,&gpdpp);
		for (j = 0; j < 3; j++) {
		    r += (gpdpp.pos[j]-xcen[j])*(gpdpp.pos[j]-xcen[j]);
		    }
		}
	    r = sqrt(r);
	    if ((r < ro) && (r >= ri)) {
		thout.ngas++;
		if (positionprecision == 0) {
		    write_tipsy_standard_gas(&xdrsout1,&gp);
		    }
		else if (positionprecision == 1) {
		    write_tipsy_standard_gas_dpp(&xdrsout1,&gpdpp);
		    }
		ap.ia[0] = 1+i;
		write_array_particle(&xdrsout2,&ahout,&ap);
		}
	    }
	for (i = 0; i < thin.ndark; i++) {
	    r = 0;
	    if (positionprecision == 0) {
		read_tipsy_standard_dark(&xdrsin1,&dp);
		for (j = 0; j < 3; j++) {
		    r += (dp.pos[j]-xcen[j])*(dp.pos[j]-xcen[j]);
		    }
		}
	    else if (positionprecision == 1) {
		read_tipsy_standard_dark_dpp(&xdrsin1,&dpdpp);
		for (j = 0; j < 3; j++) {
		    r += (dpdpp.pos[j]-xcen[j])*(dpdpp.pos[j]-xcen[j]);
		    }
		}
	    r = sqrt(r);
	    if ((r < ro) && (r >= ri)) {
		thout.ndark++;
		if (positionprecision == 0) {
		    write_tipsy_standard_dark(&xdrsout1,&dp);
		    }
		else if (positionprecision == 1) {
		    write_tipsy_standard_dark_dpp(&xdrsout1,&dpdpp);
		    }
		ap.ia[0] = thin.ngas+1+i;
		write_array_particle(&xdrsout2,&ahout,&ap);
		}
	    }
	for (i = 0; i < thin.nstar; i++) {
	    r = 0;
	    if (positionprecision == 0) {
		read_tipsy_standard_star(&xdrsin1,&sp);
		for (j = 0; j < 3; j++) {
		    r += (sp.pos[j]-xcen[j])*(sp.pos[j]-xcen[j]);
		    }
		}
	    if (positionprecision == 1) {
		read_tipsy_standard_star_dpp(&xdrsin1,&spdpp);
		for (j = 0; j < 3; j++) {
		    r += (spdpp.pos[j]-xcen[j])*(spdpp.pos[j]-xcen[j]);
		    }
		}
	    r = sqrt(r);
	    if ((r < ro) && (r >= ri)) {
		thout.nstar++;
		if (positionprecision == 0) {
		    write_tipsy_standard_star(&xdrsout1,&sp);
		    }
		else if (positionprecision == 1) {
		    write_tipsy_standard_star_dpp(&xdrsout1,&spdpp);
		    }
		ap.ia[0] = thin.ngas+thin.ndark+1+i;
		write_array_particle(&xdrsout2,&ahout,&ap);
		}
	    }
	}
    thout.ntotal = thout.ngas + thout.ndark + thout.nstar;
    ahout.N[0] = thout.ntotal;
    xdr_destroy(&xdrsin1);
    xdr_destroy(&xdrsout1);
    fclose(fout1);
    fclose(fout2);
    /*
    ** Now read temporary files again and correct header
    */
    fin1 = fopen(tempname1,"r");
    assert(fin1 != NULL);
    fin2 = fopen(tempname2,"r");
    assert(fin2 != NULL);
    xdrstdio_create(&xdrsin1,fin1,XDR_DECODE);
    xdrstdio_create(&xdrsin2,fin2,XDR_DECODE);
    sprintf(tempname1,"%s.std",outname);
    fout1 = fopen(tempname1,"w");
    assert(fout1 != NULL);
    sprintf(tempname2,"%s.index.array.std",outname);
    fout2 = fopen(tempname2,"w");
    assert(fout2 != NULL);
    xdrstdio_create(&xdrsout1,fout1,XDR_ENCODE);
    xdrstdio_create(&xdrsout2,fout2,XDR_ENCODE);
    read_tipsy_standard_header(&xdrsin1,&thin);
    write_tipsy_standard_header(&xdrsout1,&thout);
    read_array_header(&xdrsin2,&ahin);
    write_array_header(&xdrsout2,&ahout);
    /*
    ** Write particles
    */
    if (positionprecision == 0) {
	for (i = 0; i < thout.ngas; i++) { 
	    read_tipsy_standard_gas(&xdrsin1,&gp);
	    write_tipsy_standard_gas(&xdrsout1,&gp);
	    }
	for (i = 0; i < thout.ndark; i++) {
	    read_tipsy_standard_dark(&xdrsin1,&dp);
	    write_tipsy_standard_dark(&xdrsout1,&dp);
	    }
	for (i = 0; i < thout.nstar; i++) {
	    read_tipsy_standard_star(&xdrsin1,&sp);
	    write_tipsy_standard_star(&xdrsout1,&sp);
	    }
	}
    else if (positionprecision == 1) {
	for (i = 0; i < thout.ngas; i++) { 
	    read_tipsy_standard_gas_dpp(&xdrsin1,&gpdpp);
	    write_tipsy_standard_gas_dpp(&xdrsout1,&gpdpp);
	    }
	for (i = 0; i < thout.ndark; i++) {
	    read_tipsy_standard_dark_dpp(&xdrsin1,&dpdpp);
	    write_tipsy_standard_dark_dpp(&xdrsout1,&dpdpp);
	    }
	for (i = 0; i < thout.nstar; i++) {
	    read_tipsy_standard_star_dpp(&xdrsin1,&spdpp);
	    write_tipsy_standard_star_dpp(&xdrsout1,&spdpp);
	    }
	}
    /*
    ** Write array
    */
    for (i = 0; i < thout.ngas; i++) { 
	read_array_particle(&xdrsin2,&ahout,&ap);
	write_array_particle(&xdrsout2,&ahout,&ap);
	}
    for (i = 0; i < thout.ndark; i++) {
	read_array_particle(&xdrsin2,&ahout,&ap);
	write_array_particle(&xdrsout2,&ahout,&ap);
	}
    for (i = 0; i < thout.nstar; i++) {
	read_array_particle(&xdrsin2,&ahout,&ap);
	write_array_particle(&xdrsout2,&ahout,&ap);
	}
    xdr_destroy(&xdrsin1);
    xdr_destroy(&xdrsin2);
    xdr_destroy(&xdrsout1);
    xdr_destroy(&xdrsout2);
    fclose(fin1);
    fclose(fin2);
    fclose(fout1);
    fclose(fout2);
    fprintf(stderr,"Time: %g Ntotal: %d Ngas: %d Ndark: %d Nstar: %d Ndim: %d (input)\n",
	    thin.time,thin.ntotal,thin.ngas,thin.ndark,thin.nstar,thin.ndim);
    fprintf(stderr,"Time: %g Ntotal: %d Ngas: %d Ndark: %d Nstar: %d Ndim: %d (output)\n",
	    thout.time,thout.ntotal,thout.ngas,thout.ndark,thout.nstar,thout.ndim);
    exit(0);
    }

void usage(void) {

    fprintf(stderr,"\n");
    fprintf(stderr,"Cutout cuts out regions from input file.\n\n");
    fprintf(stderr,"You can specify the following arguments:\n\n");
    fprintf(stderr,"-spp           : set this flag if input and output file have single precision positions (default)\n");
    fprintf(stderr,"-dpp           : set this flag if input and output file have double precision positions\n");
    fprintf(stderr,"-rxcen <value> : x-coordinate of centre [LU] (default: 0 LU)\n");
    fprintf(stderr,"-rycen <value> : y-coordinate of centre [LU] (default: 0 LU)\n");
    fprintf(stderr,"-rzcen <value> : z-coordinate of centre [LU] (default: 0 LU)\n");
    fprintf(stderr,"-drxi <value>  : inner box side length in x-direction [LU] (default: 0 LU)\n");
    fprintf(stderr,"-dryi <value>  : inner box side length in y-direction [LU] (default: 0 LU)\n");
    fprintf(stderr,"-drzi <value>  : inner box side length in z-direction [LU] (default: 0 LU)\n");
    fprintf(stderr,"-drxo <value>  : outer box side length in x-direction [LU] (default: 0 LU)\n");
    fprintf(stderr,"-dryo <value>  : outer box side length in y-direction [LU] (default: 0 LU)\n");
    fprintf(stderr,"-drzo <value>  : outer box side length in z-direction [LU] (default: 0 LU)\n");
    fprintf(stderr,"-ri <value>    : inner shell radius [LU] (default: 0 LU)\n");
    fprintf(stderr,"-ro <value>    : outer shell radius [LU] (default: 0 LU)\n");
    fprintf(stderr,"-o <name>      : output name\n");
    fprintf(stderr,"< <name>       : input file in tipsy standard binary format\n");
    exit(1);
    }
