#ifndef ANNOT
#define ANNOT

typedef struct tagCTAB {
  int num_entries;
  int *lbl_num;
  char **lbl_name;
  int *r; // Red
  int *g; // Green 
  int *b; // Blue
  int *t; // Transparency: 1 - alpha
} CTAB;

int byterev_int(int i);
void read_int(int *i, FILE *fp);

void free_ctab(CTAB *t);
void read_ctab_from_annot(CTAB *ctab, FILE *af);
void read_ctab_from_ctab(CTAB *ctab, FILE *ctabf);
void print_ctab(CTAB *t);
int get_struc_num(CTAB *ctab, char roi_name[]);

int byterev_int(int i);
void read_int(int *i, FILE *fp);
int lblf_nvtx(FILE *lblf);
void get_lblf_vno(FILE *lblf, int *vno, int nvtx);
void write_annot(FILE *annotf, int nvtx, int *vno, int *label, 
		 char ctabfn[], CTAB *ctab);

#endif
