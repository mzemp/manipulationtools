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
    int minint, maxint;
    int arrayselection;
    int txtoutput, tsoutput, arrayfile;
    int positionprecision;
    int integerindex, floatindex, doubleindex, index;
    double min, max;
    char outname[100], tempname1[100], tempname2[100], arrayfilename[100];
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

    minint = 0;
    maxint = 0;
    min = 0;
    max = 0;
    index = -1;
    integerindex = 0;
    floatindex = 0;
    doubleindex = 0;
    txtoutput = 0;
    tsoutput = 1;
    arrayfile = 0;
    positionprecision = 0;
    shift = 0;
    delta = 1;
    fin1 = NULL;
    fin2 = NULL;
    fout1 = NULL;
    fout2 = NULL;
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
	else if (strcmp(argv[i],"-min") == 0) {
	    i++;
	    if (i >= argc) {
		usage();
		}
	    min = atof(argv[i]);
	    i++;
	    }
	else if (strcmp(argv[i],"-max") == 0) {
	    i++;
	    if (i >= argc) {
		usage();
		}
	    max = atof(argv[i]);
	    i++;
	    }
	else if (strcmp(argv[i],"-format") == 0) {
	    i++;
	    if (i >= argc) {
		usage();
		}
	    if (strcmp(argv[i],"txt") == 0) {
		txtoutput = 1;
		tsoutput = 0;
		}
	    else if (strcmp(argv[i],"ts") == 0) {
		txtoutput = 0;
		tsoutput = 1;
		}
	    else {
		usage();
		}
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
        else if (strcmp(argv[i],"-array") == 0) {
            i++;
            if (i >= argc) {
                usage();
                }
            arrayfile = 1;
            strcpy(arrayfilename,argv[i]);
            i++;
            }
	else if ((strcmp(argv[i],"-h") == 0) || (strcmp(argv[i],"-help") == 0)) {
	    usage();
	    }
	else {
	    usage();
	    }
	}
    if (integerindex == 1) {
	minint = (int) min;
	maxint = (int) max;
	}
    /*
    ** Read & write particles
    */
    xdrstdio_create(&xdrsin1,stdin,XDR_DECODE);
    read_tipsy_standard_header(&xdrsin1,&thin);
    if (txtoutput == 1) {
	sprintf(tempname1,"%s.extract.txt",outname);
	fout1 = fopen(tempname1,"w");
	assert(fout1 != NULL);
	thout.ngas = 0;
	thout.ndark = 0;
	thout.nstar = 0;
	}
    else if (tsoutput == 1) {
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
	}
    if (arrayfile == 1) {
        fin2 = fopen(arrayfilename,"r");
        assert(fin2 != NULL);
        xdrstdio_create(&xdrsin2,fin2,XDR_DECODE);
        read_array_header(&xdrsin2,&ahin);
	assert(ahin.N[0] == thin.ntotal);
	/*
	** Write out temporary file to be replaced later
	*/
	sprintf(tempname2,"tf2_%s",outname);
	fout2 = fopen(tempname2,"w");
	assert(fout2 != NULL);
	xdrstdio_create(&xdrsout2,fout2,XDR_ENCODE);
	ahout.N[0] = 0;
	ahout.N[1] = ahin.N[1];
	ahout.N[2] = ahin.N[2];
	ahout.N[3] = ahin.N[3];
	allocate_array_particle(&ahout,&ap);
	write_array_header(&xdrsout2,&ahout);
	}
    if (positionprecision == 0) {
	for (i = 0; i < thin.ngas; i++) {
	    read_tipsy_standard_gas(&xdrsin1,&gp);
	    arrayselection = 1;
	    if (arrayfile == 1) {
		read_array_particle(&xdrsin2,&ahin,&ap);
		arrayselection = 0;
		if (integerindex == 1) {
		    if ((ap.ia[index] <= maxint) && (ap.ia[index] >= minint)) {
			arrayselection = 1;
			}
		    }
		else if (floatindex == 1) {
		    if ((ap.fa[index] <= max) && (ap.fa[index] >= min)) {
			arrayselection = 1;
			}
		    }
		else if (doubleindex == 1) {
		    if ((ap.da[index] <= max) && (ap.da[index] >= min)) {
			arrayselection = 1;
			}
		    }
		}
	    if (((i+1-shift)%delta == 0) && arrayselection) {
		thout.ngas++;
		if (txtoutput == 1) {
		    fprintf(fout1,"%+.6e %+.6e %+.6e %+.6e %+.6e %+.6e ",gp.pos[0],gp.pos[1],gp.pos[2],gp.vel[0],gp.vel[1],gp.vel[2]);
		    fprintf(fout1,"\n");
		    }
		else if (tsoutput == 1) {
		    write_tipsy_standard_gas(&xdrsout1,&gp);
		    if (arrayfile == 1) {
			ahout.N[0]++;
			write_array_particle(&xdrsout2,&ahout,&ap);
			}
		    }
		}
	    }
	for (i = 0; i < thin.ndark; i++) {
	    read_tipsy_standard_dark(&xdrsin1,&dp);
	    arrayselection = 1;
	    if (arrayfile == 1) {
		read_array_particle(&xdrsin2,&ahin,&ap);
		arrayselection = 0;
		if (integerindex == 1) {
		    if ((ap.ia[index] <= maxint) && (ap.ia[index] >= minint)) {
			arrayselection = 1;
			}
		    }
		else if (floatindex == 1) {
		    if ((ap.fa[index] <= max) && (ap.fa[index] >= min)) {
			arrayselection = 1;
			}
		    }
		else if (doubleindex == 1) {
		    if ((ap.da[index] <= max) && (ap.da[index] >= min)) {
			arrayselection = 1;
			}
		    }
		}
	    if (((i+1-shift)%delta == 0) && arrayselection) {
		thout.ndark++;
		if (txtoutput == 1) {
		    fprintf(fout1,"%+.6e %+.6e %+.6e %+.6e %+.6e %+.6e ",dp.pos[0],dp.pos[1],dp.pos[2],dp.vel[0],dp.vel[1],dp.vel[2]);
		    fprintf(fout1,"\n");
		    }
		else if (tsoutput == 1) {
		    write_tipsy_standard_dark(&xdrsout1,&dp);
		    if (arrayfile == 1) {
			ahout.N[0]++;
			write_array_particle(&xdrsout2,&ahout,&ap);
			}
		    }
		}
	    }
	for (i = 0; i < thin.nstar; i++) {
	    read_tipsy_standard_star(&xdrsin1,&sp);
	    arrayselection = 1;
	    if (arrayfile == 1) {
		read_array_particle(&xdrsin2,&ahin,&ap);
		arrayselection = 0;
		if (integerindex == 1) {
		    if ((ap.ia[index] <= maxint) && (ap.ia[index] >= minint)) {
			arrayselection = 1;
			}
		    }
		else if (floatindex == 1) {
		    if ((ap.fa[index] <= max) && (ap.fa[index] >= min)) {
			arrayselection = 1;
			}
		    }
		else if (doubleindex == 1) {
		    if ((ap.da[index] <= max) && (ap.da[index] >= min)) {
			arrayselection = 1;
			}
		    }
		}
	    if (((i+1-shift)%delta == 0) && arrayselection) {
		thout.nstar++;
		if (txtoutput == 1) {
		    fprintf(fout1,"%+.6e %+.6e %+.6e %+.6e %+.6e %+.6e ",sp.pos[0],sp.pos[1],sp.pos[2],sp.vel[0],sp.vel[1],sp.vel[2]);
		    fprintf(fout1,"\n");
		    }
		else if (tsoutput == 1) {
		    write_tipsy_standard_star(&xdrsout1,&sp);
		    if (arrayfile == 1) {
			ahout.N[0]++;
			write_array_particle(&xdrsout2,&ahout,&ap);
			}
		    }
		}
	    }
	}
    else if (positionprecision == 1) {
	for (i = 0; i < thin.ngas; i++) {
	    read_tipsy_standard_gas_dpp(&xdrsin1,&gpdpp);
	    arrayselection = 1;
	    if (arrayfile == 1) {
		read_array_particle(&xdrsin2,&ahin,&ap);
		arrayselection = 0;
		if (integerindex == 1) {
		    if ((ap.ia[index] <= maxint) && (ap.ia[index] >= minint)) {
			arrayselection = 1;
			}
		    }
		else if (floatindex == 1) {
		    if ((ap.fa[index] <= max) && (ap.fa[index] >= min)) {
			arrayselection = 1;
			}
		    }
		else if (doubleindex == 1) {
		    if ((ap.da[index] <= max) && (ap.da[index] >= min)) {
			arrayselection = 1;
			}
		    }
		}
	    if (((i+1-shift)%delta == 0) && arrayselection) {
		thout.ngas++;
		if (txtoutput == 1) {
		    fprintf(fout1,"%+.14e %+.14e %+.14e %+.6e %+.6e %+.6e ",gpdpp.pos[0],gpdpp.pos[1],gpdpp.pos[2],gpdpp.vel[0],gpdpp.vel[1],gpdpp.vel[2]);
		    fprintf(fout1,"\n");
		    }
		else if (tsoutput == 1) {
		    write_tipsy_standard_gas_dpp(&xdrsout1,&gpdpp);
		    if (arrayfile == 1) {
			ahout.N[0]++;
			write_array_particle(&xdrsout2,&ahout,&ap);
			}
		    }
		}
	    }
	for (i = 0; i < thin.ndark; i++) {
	    read_tipsy_standard_dark_dpp(&xdrsin1,&dpdpp);
	    arrayselection = 1;
	    if (arrayfile == 1) {
		read_array_particle(&xdrsin2,&ahin,&ap);
		arrayselection = 0;
		if (integerindex == 1) {
		    if ((ap.ia[index] <= maxint) && (ap.ia[index] >= minint)) {
			arrayselection = 1;
			}
		    }
		else if (floatindex == 1) {
		    if ((ap.fa[index] <= max) && (ap.fa[index] >= min)) {
			arrayselection = 1;
			}
		    }
		else if (doubleindex == 1) {
		    if ((ap.da[index] <= max) && (ap.da[index] >= min)) {
			arrayselection = 1;
			}
		    }
		}
	    if (((i+1-shift)%delta == 0) && arrayselection) {
		thout.ndark++;
		if (txtoutput == 1) {
		    fprintf(fout1,"%+.14e %+.14e %+.14e %+.6e %+.6e %+.6e ",dpdpp.pos[0],dpdpp.pos[1],dpdpp.pos[2],dpdpp.vel[0],dpdpp.vel[1],dpdpp.vel[2]);
		    fprintf(fout1,"\n");
		    }
		else if (tsoutput == 1) {
		    write_tipsy_standard_dark_dpp(&xdrsout1,&dpdpp);
		    if (arrayfile == 1) {
			ahout.N[0]++;
			write_array_particle(&xdrsout2,&ahout,&ap);
			}
		    }
		}
	    }
	for (i = 0; i < thin.nstar; i++) {
	    read_tipsy_standard_star_dpp(&xdrsin1,&spdpp);
	    arrayselection = 1;
	    if (arrayfile == 1) {
		read_array_particle(&xdrsin2,&ahin,&ap);
		arrayselection = 0;
		if (integerindex == 1) {
		    if ((ap.ia[index] <= maxint) && (ap.ia[index] >= minint)) {
			arrayselection = 1;
			}
		    }
		else if (floatindex == 1) {
		    if ((ap.fa[index] <= max) && (ap.fa[index] >= min)) {
			arrayselection = 1;
			}
		    }
		else if (doubleindex == 1) {
		    if ((ap.da[index] <= max) && (ap.da[index] >= min)) {
			arrayselection = 1;
			}
		    }
		}
	    if (((i+1-shift)%delta == 0) && arrayselection) {
		thout.nstar++;
		if (txtoutput == 1) {
		    fprintf(fout1,"%+.14e %+.14e %+.14e %+.6e %+.6e %+.6e ",spdpp.pos[0],spdpp.pos[1],spdpp.pos[2],spdpp.vel[0],spdpp.vel[1],spdpp.vel[2]);
		    fprintf(fout1,"\n");
		    }
		else if (tsoutput == 1) {
		    write_tipsy_standard_star_dpp(&xdrsout1,&spdpp);
		    if (arrayfile == 1) {
			ahout.N[0]++;
			write_array_particle(&xdrsout2,&ahout,&ap);
			}
		    }
		}
	    }
	}
    thout.ntotal = thout.ngas + thout.ndark + thout.nstar;
    xdr_destroy(&xdrsin1);
    if (txtoutput == 1) {
	fclose(fout1);
	}
    else if (tsoutput == 1) {
	xdr_destroy(&xdrsout1);
	}
    if (arrayfile == 1) {
	assert(thout.ntotal == ahout.N[0]);
	xdr_destroy(&xdrsin2);
	if (tsoutput == 1) {
	    xdr_destroy(&xdrsout2);
	    }
	}
    /*
    ** Give some output
    */
    fprintf(stderr,"In:  %g %d %d %d %d\n",thin.time,thin.ntotal,thin.ngas,thin.ndark,thin.nstar);
    fprintf(stderr,"Out: %g %d %d %d %d\n",thout.time,thout.ntotal,thout.ngas,thout.ndark,thout.nstar);
    fprintf(stderr,"Array: min %g max %g\n",min,max);
    /*
    ** Now read temporary files again and correct header for tipsy standard output
    */
    if (tsoutput == 1) {
	fin1 = fopen(tempname1,"r");
	assert(fin1 != NULL);
	xdrstdio_create(&xdrsin1,fin1,XDR_DECODE);
	sprintf(tempname1,"%s.extract.std",outname);
	fout1 = fopen(tempname1,"w");
	assert(fout1 != NULL);
	xdrstdio_create(&xdrsout1,fout1,XDR_ENCODE);
	read_tipsy_standard_header(&xdrsin1,&thin);
	write_tipsy_standard_header(&xdrsout1,&thout);
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
	xdr_destroy(&xdrsin1);
	xdr_destroy(&xdrsout1);
	fclose(fin1);
	fclose(fout1);
	if (arrayfile == 1) {
	    fin2 = fopen(tempname2,"r");
	    assert(fin2 != NULL);
	    xdrstdio_create(&xdrsin2,fin2,XDR_DECODE);
	    sprintf(tempname2,"%s.extract.array.std",outname);
	    fout2 = fopen(tempname2,"w");
	    assert(fout2 != NULL);
	    xdrstdio_create(&xdrsout2,fout2,XDR_ENCODE);
	    read_array_header(&xdrsin2,&ahin);
	    write_array_header(&xdrsout2,&ahout);
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
	    xdr_destroy(&xdrsin2);
	    xdr_destroy(&xdrsout2);
	    fclose(fin2);
	    fclose(fout2);
	    }
	}
    exit(0);
    }

void usage(void) {

    fprintf(stderr,"\n");
    fprintf(stderr,"Program extracts particles if the index i satisfies (i+shift) mod delta == 0\n");
    fprintf(stderr,"and (optional) if the array value v of the particle satisfies: min <= v <= max.\n\n");
    fprintf(stderr,"You can specify the following arguments:\n\n");
    fprintf(stderr,"-spp             : set this flag if input and output file have single precision positions (default)\n");
    fprintf(stderr,"-dpp             : set this flag if input and output file have double precision positions\n");
    fprintf(stderr,"-shift <value>   : index shift (default: 0)\n");
    fprintf(stderr,"-delta <value>   : index delta (default: 1)\n");
    fprintf(stderr,"-array <name>    : array file\n");
    fprintf(stderr,"-index <type><n> : <type>: i (int), f (float) or d (double), <n> array index\n");
    fprintf(stderr,"-min <value>     : min array value (default: 0)\n");	  
    fprintf(stderr,"-max <value>     : max array value (default: 0)\n");	  
    fprintf(stderr,"-format <type>   : output format <type>: txt or ts (default)\n");	  
    fprintf(stderr,"-o <name>        : output file name base\n");	       
    fprintf(stderr,"< <name>         : input file in tipsy standard binary format\n");
    fprintf(stderr,"\n");
    exit(1);
    }
