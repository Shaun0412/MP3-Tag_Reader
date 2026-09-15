#ifndef MP3TAGREADER_H
#define MP3TAGREADER_H

int mymain(int ch1,int ch2,char *newcont,char *filename);
void edit(char *,char *,char *);
void editingfun(FILE *fptr,char *filename,char *newcont,int prevsize);
void view(char * filename);
void readcontent(FILE *fptr,int size,char *str);
int readsize(FILE *fptr);
int swap(unsigned int num);
void myprint(char *ptr,int size,int opt);
void help();
void invalidarguments();

#endif