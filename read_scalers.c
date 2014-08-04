#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sca_read.h"

int main(int argc, char * argv[])
{
  // open scaler file
  FILE * fp;
  char filename[64];
  if(argc!=2)
  {
    fprintf(stderr,"Usage: sca_read_bin.o [*.sca filename]\n");
    return 0;
  }
  else
  {
    strcpy(filename,argv[1]);
    if((fp=fopen(filename,"r"))==NULL)
    {
      fprintf(stderr,"ERROR: not able to open %s\n",filename);
      return 0;
    };
  };
  int runnum, boardnum, unixtime;
  sscanf(filename,"sca2011t/bd%d/run%d_%d_%d.sca",&boardnum,&runnum,&boardnum,&unixtime);
  printf("reading %s\n",filename);
  printf("runnum=%d\nboard=%d\n",runnum,boardnum);


  // initialise scaler counters
  unsigned long long BBC[8][120];
  unsigned long long ZDC[8][120];
  unsigned long long VPD[8][120];
  unsigned long long bx_cnt[120];
  int b, d;
  for(b=0; b<120; b++)
  {
    bx_cnt[b]=0;
    for(d=0; d<8; d++)
    {
      BBC[d][b]=0;
      ZDC[d][b]=0;
      VPD[d][b]=0;
    };
  };

  
  // read scaler file and increment counters
  // -- see 24bit.pdf
  int num, chn, bx;
  int cBBC, cZDC, cVPD;
  unsigned long long val;
  while(sca_read_bin(0, fp, &num, &chn, &val)==1)
  {
    if(!(num==1 && chn==0))
    {
      bx = (chn>>17) & 0x7F; // mask bits 17-23
      chn = chn & 0x1FFFF;   // mask bits 0-16

      cBBC = ((chn>>10)&0x3) | ((chn>>14)&0x4); // BBC [XWE]
      cZDC = ((chn>>8)&0x3) | ((chn>>13)&0x4);  // ZDC [XWE]
      cVPD = (chn>>12)&0x4; // ZDC [XWE] (note: no singles bits)

      //printf("chn=%x cBBC=%x cZDC=%x cVPD=%x\n",chn,cBBC,cZDC,cVPD);

      if(bx>=0 && bx<=119)
      {
        if( cBBC == 0x0 ) BBC[0][bx]+=val;
        else if( cBBC == 0x1 ) BBC[1][bx]+=val;
        else if( cBBC == 0x2 ) BBC[2][bx]+=val;
        else if( cBBC == 0x3 ) BBC[3][bx]+=val;
        else if( cBBC == 0x4 ) BBC[4][bx]+=val;
        else if( cBBC == 0x5 ) BBC[5][bx]+=val;
        else if( cBBC == 0x6 ) BBC[6][bx]+=val;
        else if( cBBC == 0x7 ) BBC[7][bx]+=val;

        if( cZDC == 0x0 ) ZDC[0][bx]+=val;
        else if( cZDC == 0x1 ) ZDC[1][bx]+=val;
        else if( cZDC == 0x2 ) ZDC[2][bx]+=val;
        else if( cZDC == 0x3 ) ZDC[3][bx]+=val;
        else if( cZDC == 0x4 ) ZDC[4][bx]+=val;
        else if( cZDC == 0x5 ) ZDC[5][bx]+=val;
        else if( cZDC == 0x6 ) ZDC[6][bx]+=val;
        else if( cZDC == 0x7 ) ZDC[7][bx]+=val;

        if( cVPD == 0x0 ) VPD[0][bx]+=val;
        else if( cVPD == 0x1 ) VPD[1][bx]+=val;
        else if( cVPD == 0x2 ) VPD[2][bx]+=val;
        else if( cVPD == 0x3 ) VPD[3][bx]+=val;
        else if( cVPD == 0x4 ) VPD[4][bx]+=val;
        else if( cVPD == 0x5 ) VPD[5][bx]+=val;
        else if( cVPD == 0x6 ) VPD[6][bx]+=val;
        else if( cVPD == 0x7 ) VPD[7][bx]+=val;

        bx_cnt[bx]+=val;
      };
    };
  };

  fclose(fp);


  // print scaler counts to datfile
  char datfile[128];
  sprintf(datfile,"datfiles/run%d_%d_%d.dat",runnum,boardnum,unixtime);
  FILE * fo;
  fo=fopen(datfile,"w");
  for(b=0; b<120; b++)
  {
    fprintf(fo,"%d",b);
    for(d=0; d<8; d++) fprintf(fo," %lld",BBC[d][b]);
    for(d=0; d<8; d++) fprintf(fo," %lld",ZDC[d][b]);
    for(d=0; d<8; d++) fprintf(fo," %lld",VPD[d][b]);
    fprintf(fo," %lld\n",bx_cnt[b]);
  };
  fclose(fo);


  return 1;
}
