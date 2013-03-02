#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "annot.h"

int byterev_int(int i) {
  int byte1 = i & 0x000000FF;
  int byte2 = (i & 0x0000FF00) >> 8;
  int byte3 = (i & 0x00FF0000) >> 16;
  int byte4 = (i & 0xFF000000) >> 24;
  
  //printf("byte1 = %x\n", byte1);
  //printf("byte2 = %x\n", byte2);
  //printf("byte3 = %x\n", byte3);
  //printf("byte4 = %x\n", byte4);
  
  return ((byte1 << 24) + (byte2 << 16) + (byte3 << 8) + (byte4));
}

void read_int(int *i, FILE *fp) {
  fread(i, sizeof(int), 1, fp);
  //printf("i = %d\n", *i);
  *i = byterev_int(*i);
}

/* Get the number of verticies in a label file */
int lblf_nvtx(FILE *lblf) {
  int i0;
  char c0;

  c0 = '\0';
  while (c0 != '\n' && c0 != '\r') {
    c0 = fgetc(lblf);
  }
  fscanf(lblf, "%d", &i0);

  return i0;
}

/* Get all vertex numbers (vno) from a label file */
void get_lblf_vno(FILE *lblf, int *vno, int nvtx) {
  int i0, i1, i;
  float f0;
  char c0;
  
  c0 = '\0';
  for (i0 = 0; i0 < 2; ++i0) {
    while (c0 != '\n' && c0 != '\r') {
      c0 = fgetc(lblf);
    }
  }

  for (i0 = 0; i0 < nvtx; i0++) {
    fscanf(lblf, "%d", &i);
    //printf("Read vertex #%d\n", i);
    vno[i0] = i;
    
    for (i1 = 0; i1 < 4; ++i1) 
      fscanf(lblf, "%f", &f0);
  }
}

/* Write vertices data (only) to annot file */
void write_annot(FILE *annotf, int nvtx, int *vno, int *label, 
		 char ctabfn[], CTAB *ctab) { 
  int i0, ii;
  int maxstruc, slen;

  printf("write_annot: nvtx = %d\n", nvtx);

  ii = byterev_int(nvtx);
  fwrite(&ii, sizeof(int), 1, annotf);

  for (i0 = 0; i0 < nvtx; ++i0) {
    ii = byterev_int(vno[i0]);
    fwrite(&ii, sizeof(int), 1, annotf);
    
    ii = byterev_int(label[i0]);
    fwrite(&ii, sizeof(int), 1, annotf);
  }

  /* LUT part */
  /* tag */
  i0 = 1;
  ii = byterev_int(i0);
  fwrite(&ii, sizeof(int), 1, annotf); 

  /* ctabversion */
  ii = 0xfffffffe;
  ii = byterev_int(ii);
  fwrite(&ii, sizeof(int), 1, annotf);

  /* maxstruc */
  maxstruc = ctab->num_entries;
  //for (i0 = 0; i0 < nvtx; ++i0)
  //  if (label[i0] > maxstruc)
  //    maxstruc = label[i0];
  ii = byterev_int(maxstruc);
  fwrite(&ii, sizeof(int), 1, annotf);
  
  //printf("maxstruct = %d\n", maxstruc);

  /* slen: length of the following file name */
  slen = strlen(ctabfn) + 1; //Includes the termination char
  ii = byterev_int(slen);
  fwrite(&ii, sizeof(int), 1, annotf);

  //printf("slen = %d\n", slen);

  /* origctabfn */
  fwrite(ctabfn, sizeof(char), slen, annotf);

  /* num_entries */
  ii = byterev_int(ctab->num_entries);
  fwrite(&ii, sizeof(int), 1, annotf);
  
  /* Main part of the LUT */
  for (i0 = 0; i0 < ctab->num_entries; ++i0) {
    ii = byterev_int(ctab->lbl_num[i0]);
    fwrite(&ii, sizeof(int), 1, annotf);
    
    slen = strlen(ctab->lbl_name[i0]) + 1; //Includes the termination char
    ii = byterev_int(slen);
    fwrite(&ii, sizeof(int), 1, annotf);

    fwrite(ctab->lbl_name[i0], sizeof(char), slen, annotf);

    ii = byterev_int(ctab->r[i0]);
    fwrite(&ii, sizeof(int), 1, annotf);
    
    ii = byterev_int(ctab->g[i0]);
    fwrite(&ii, sizeof(int), 1, annotf);

    ii = byterev_int(ctab->b[i0]);
    fwrite(&ii, sizeof(int), 1, annotf);

    ii = byterev_int(ctab->t[i0]);
    fwrite(&ii, sizeof(int), 1, annotf);
  }
}

