/* 
** rotate.c
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

void MatrixMultiplication(double R[3][3], double vin[3], double vout[3]) {

    int i, j;

    for (i = 0; i < 3; i++) {
	vout[i] = 0;
	for (j = 0; j < 3; j++) {
	    vout[i] += R[i][j]*vin[j];
	    }
	}
    }

int main(int argc, char **argv) {

    int i, j;
    int rotmode, rotdir;
    double phi, theta, psi;
    double cosphi, costheta, cospsi;
    double sinphi, sintheta, sinpsi;
    double R[3][3], vin[3], vout[3], temp;
    TIPSY_HEADER th;
    TIPSY_GAS_PARTICLE gp;
    TIPSY_DARK_PARTICLE dp;
    TIPSY_STAR_PARTICLE sp;
    XDR xdrsin, xdrsout;

    /*
    ** Set default values
    */

    rotmode = 0; /* zxz = 0, zyz = 1 */
    rotdir = 1; /* forward = 0 backward = 1 */
    phi = 0;
    theta = 0;
    psi = 0;

    /*
    ** Read in arguments
    */

    i = 1;
    while (i < argc) {
	if (strcmp(argv[i],"-zxz") == 0) {
	    rotmode = 0;
	    i++;
	    }
	else if (strcmp(argv[i],"-zyz") == 0) {
	    rotmode = 1;
	    i++;
	    }
	else if (strcmp(argv[i],"-fwd") == 0) {
	    rotdir = 0;
	    i++;
	    }
	else if (strcmp(argv[i],"-bwd") == 0) {
	    rotdir = 1;
	    i++;
	    }
	else if (strcmp(argv[i],"-phi") == 0) {
	    i++;
	    if (i >= argc) {
		usage();
		}
	    phi = atof(argv[i]);
	    i++;
	    if (i < argc) {
		if (strcmp(argv[i],"deg") == 0) {
		    phi *= M_PI/180.0;
		    i++;
		    }
		else if (strcmp(argv[i],"rad") == 0) {
		    i++;
		    }
		}
	    }
	else if (strcmp(argv[i],"-theta") == 0) {
	    i++;
	    if (i >= argc) {
		usage();
		}
	    theta = atof(argv[i]);
	    i++;
	    if (i < argc) {
		if (strcmp(argv[i],"deg") == 0) {
		    theta *= M_PI/180.0;
		    i++;
		    }
		else if (strcmp(argv[i],"rad") == 0) {
		    i++;
		    }
		}
	    }
	else if (strcmp(argv[i],"-psi") == 0) {
	    i++;
	    if (i >= argc) {
		usage();
		}
	    psi = atof(argv[i]);
	    i++;
	    if (i < argc) {
		if (strcmp(argv[i],"deg") == 0) {
		    psi *= M_PI/180.0;
		    i++;
		    }
		else if (strcmp(argv[i],"rad") == 0) {
		    i++;
		    }
		}
	    }
	else if ((strcmp(argv[i],"-h") == 0) || (strcmp(argv[i],"-help") == 0)) {
	    usage();
	    }
	else {
	    usage();
	    }
	}

    /*
    ** Internally we do a zxz rotation
    ** => convert angles if zyz rotation input
    */

    assert((theta >= 0) && (theta <= M_PI));
    if (rotmode == 1) {
	phi = phi - 3*M_PI/2.0;
	psi = psi - M_PI/2.0;
	}

    cosphi = cos(phi);
    costheta = cos(theta);
    cospsi = cos(psi);
    sinphi = sin(phi);
    sintheta = sin(theta);
    sinpsi = sin(psi);

    R[0][0] = cosphi*cospsi - sinphi*costheta*sinpsi;
    R[0][1] = -cosphi*sinpsi - sinphi*costheta*cospsi;
    R[0][2] = sinphi*sintheta;

    R[1][0] = sinphi*cospsi + cosphi*costheta*sinpsi;
    R[1][1] = -sinphi*sinpsi + cosphi*costheta*cospsi;
    R[1][2] = -cosphi*sintheta;

    R[2][0] = sintheta*sinpsi;
    R[2][1] = sintheta*cospsi;
    R[2][2] = costheta;

    if (rotdir == 1) {
	temp = R[0][1];
	R[0][1] = R[1][0];
	R[1][0] = temp;
	temp = R[0][2];
	R[0][2] = R[2][0];
	R[2][0] = temp;
	temp = R[1][2];
	R[1][2] = R[2][1];
	R[2][1] = temp;
	}

    /*
    ** Read in and write out header
    */

    xdrstdio_create(&xdrsin,stdin,XDR_DECODE);
    read_tipsy_xdr_header(&xdrsin,&th);
    xdrstdio_create(&xdrsout,stdout,XDR_ENCODE);
    write_tipsy_xdr_header(&xdrsout,&th);

    /*
    ** Add rotation to particles and write them out
    */

    for(i = 0; i < th.ngas; i++) {
	read_tipsy_xdr_gas(&xdrsin,&gp);
	for(j = 0; j < 3; j++) {
	    vin[j] = gp.pos[j];
	    }
	MatrixMultiplication(R,vin,vout);
	for(j = 0; j < 3; j++) {
	    gp.pos[j] = vout[j];
	    vin[j] = gp.vel[j];
	    }
	MatrixMultiplication(R,vin,vout);
	for(j = 0; j < 3; j++) {
	    gp.vel[j] = vout[j];
	    }
	write_tipsy_xdr_gas(&xdrsout,&gp);
	}
    for(i = 0; i < th.ndark; i++) {
	read_tipsy_xdr_dark(&xdrsin,&dp);
	for(j = 0; j < 3; j++) {
	    vin[j] = dp.pos[j];
	    }
	MatrixMultiplication(R,vin,vout);
	for(j = 0; j < 3; j++) {
	    dp.pos[j] = vout[j];
	    vin[j] = dp.vel[j];
	    }
	MatrixMultiplication(R,vin,vout);
	for(j = 0; j < 3; j++) {
	    dp.vel[j] = vout[j];
	    }
	write_tipsy_xdr_dark(&xdrsout,&dp);
	}
    for(i = 0; i < th.nstar; i++) {
	read_tipsy_xdr_star(&xdrsin,&sp);
	for(j = 0; j < 3; j++) {
	    vin[j] = sp.pos[j];
	    }
	MatrixMultiplication(R,vin,vout);
	for(j = 0; j < 3; j++) {
	    sp.pos[j] = vout[j];
	    vin[j] = sp.vel[j];
	    }
	MatrixMultiplication(R,vin,vout);
	for(j = 0; j < 3; j++) {
	    sp.vel[j] = vout[j];
	    }
	write_tipsy_xdr_star(&xdrsout,&sp);
	}

    xdr_destroy(&xdrsin);
    xdr_destroy(&xdrsout);  

    fprintf(stderr,"Time: %g Ntotal: %d Ngas: %d Ndark: %d Nstar: %d Ndim: %d\n",
	    th.time,th.ntotal,th.ngas,th.ndark,th.nstar,th.ndim);
    fprintf(stderr,"Angles for zxz rotation phi: %g rad theta: %g rad psi: %g rad\n",phi,theta,psi);

    exit(0);
    }

void usage(void) {

    fprintf(stderr,"\n");
    fprintf(stderr,"This program rotates the input structure around (0,0,0) according to the Euler angles phi, theta, psi.\n");
    fprintf(stderr,"Internally, a zxz rotation is done but you can also provide zyz Euler angles by setting the -zyz flag\n");
    fprintf(stderr,"and the code then converts it automatically to zxz rotation Euler angles.\n\n");
    fprintf(stderr,"You can specify the following arguments:\n\n");
    fprintf(stderr,"-zxz                  : set this flag if your Euler angles are for a zxz rotation (default)\n");
    fprintf(stderr,"-zyz                  : set this flag if your Euler angles are for a zyz rotation\n");
    fprintf(stderr,"-fwd                  : set this flag if your rotation is forwards (r => r''')\n");
    fprintf(stderr,"-bwd                  : set this flag if your rotation is backwards (r''' => r) (default)\n");
    fprintf(stderr,"-phi <value> <unit>   : 1. Euler angle (default: 0 rad), <unit>: deg or rad (optional - default: rad)\n");
    fprintf(stderr,"-theta <value> <unit> : 2. Euler angle (default: 0 rad), <unit>: deg or rad (optional - default: rad)\n");
    fprintf(stderr,"-psi <value> <unit>   : 3. Euler angle (default: 0 rad), <unit>: deg or rad (optional - default: rad)\n");
    fprintf(stderr,"< <name>              : input file in tipsy standard binary format\n");
    fprintf(stderr,"> <name>              : output file in tipsy standard binary format\n");
    fprintf(stderr,"\n");
    exit(1);
    }
