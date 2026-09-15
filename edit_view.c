#include<stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include<unistd.h>
#include "mp3tagreader.h"

void myprint(char *ptr,int size,int opt)    // function used to print on the basis of the string
{   
    if(opt==0)
        printf("\nTitle\t: ");
    else if(opt==1)
        printf("\nArtist\t: ");
    else if(opt==2)
        printf("\nAlbum\t: ");
    else if(opt==3)
         printf("\nyear\t: ");
    else if(opt==4)
        printf("\nMusic\t: ");
    else if(opt==5)
        printf("\nComment\t: ");    
    int i;
    for(i=0;i<size;i++)
    {
        printf("%c",ptr[i]);
    }
}
int swap(unsigned int num)                                  //swaping to convert from big endian to little endian vice vers
{
    unsigned int temp1,temp2,temp3,temp4;
    temp1=(num&(((1<<8)-1)<<24))>>24;                              //here we have used  bitwise opertion to eavalueat
    temp2=(num&(((1<<8)-1)<<16))>>8;
    temp3=(num&(((1<<8)-1)<<8))<<8;
    temp4=(num&((1<<8)-1))<<24;
    num=(temp1|temp2|temp3|temp4);
    return num;
}
int readsize(FILE *fptr)        //function to read the size of the contetnt present in the tag
{   
    int size;
    fread(&size,4,1,fptr);
    size=swap(size);
    return size-1;
}
void readcontent(FILE *fptr,int size,char *str)         //function that uses the size to read the content in binaray to the reqested string
{   
    fseek(fptr,3,SEEK_CUR);
    fread(str,size,1,fptr);
    *(str+size)='\0';
}

void view(char * filename)
{
    FILE *fptr;                                               //declaration of file pointer and opening of the file
    fptr=fopen(filename,"r");
    if(fptr==NULL)
    {
        fprintf(stderr,"file is not found\n");                  //if no such file, file is not found is displayed
        return;
    }
    char title[100],artist[100],album[100],year[6],content[200],composer[100];
    int sizetitle=1,sizeartist=2,sizealbum=0,sizeyear=0,sizecontent=0,sizecomposer=0;
    int *sizegroup[6]={&sizetitle,&sizeartist,&sizealbum,&sizeyear,&sizecontent,&sizecomposer};//declaration of pointer pointing to the array
    char *contentbox[6]={title,artist,album,year,content,composer};                             //content box that stores all strings
    char *tags[6]={"TIT2","TPE1","TALB","TYER","TCON","TCOM"};                                  //stores the tags to be printed
    int count=0;
    fseek(fptr,10,SEEK_CUR);                                    //its used to skip the file pointer from header
    printf("-----------------------------------------------------------------------");
    do{ 
        char tag[5];       
        fread(tag,4,1,fptr);                                    //reads the 4 bytes of the tag
        tag[4]='\0';
        int j=0;
        for(j=0;j<6;j++)
        {
            if(strcmp(tag,tags[j])==0)
            {
                *sizegroup[j]=readsize(fptr);                           //functin called to read the size of the tag that has been entered
                readcontent(fptr,*sizegroup[j],contentbox[j]);          //funciton called to read the content and store in a respective string
                myprint(contentbox[j],*sizegroup[j],j);                 //call of print funciont
            }
        }
        count++;
    }while(count<6);    ///in sample.mp3 it  has  only 5 tags
    printf("\n-----------------------------------------------------------------------\n");
    fclose(fptr);
}
void editingfun(FILE *fptr,char *filename,char *newcont,int prevsize)       //function that would edit the contact
{
    char flag[4];
    fread(flag,2,1,fptr);                                                   //reads the flag and is stored
    flag[2]='\0';
    int num=(ftell(fptr)-6);
    FILE *fp;
    fp=fopen("newsample.mp3","w+");                                          //use of another dumy file to copy the content upto the tag to be edited
    if(fp==NULL)
    {
        fprintf(stderr,"new sample file is not opening and not created\n");
        return;
    }
    char str[1000];
    fseek(fptr,0,SEEK_SET);                                     //brings the courser back to the beggining
    fread(str,num,1,fptr);                                      //reads all the content upto tag in a string
    fwrite(str,num,1,fp);                                       //writes the string content into the new folder
    int length=(strlen(newcont)+1);                             //measures the length of new content 
    int swaplength=swap(length);                                //swap the lenght from big to little endian to store in a file
    char ch='\0';
    fwrite(&swaplength,4,1,fp);                                 //storing the size in a file
    fwrite(flag,2,1,fp);                                        //flag is stored
    fwrite(&ch,1,1,fp);                                         //null character added initally itself
    fwrite(newcont,(length-1),1,fp);                            //write of new content to be added
    fseek(fptr,(prevsize+3+4),SEEK_CUR);                        //surpases the poiner of old file to next tag
    while(fread(&ch,1,1,fptr) > 0)    // reads everything upto EOF even considering EOF //return type of the fread is number of parts read
    {
        fwrite(&ch,1,1,fp);
    }
    fclose(fptr);fclose(fp);
    remove(filename);                                               //once edited file is created older file is deleted
    rename("newsample.mp3",filename);                               //change of the new file name to old name of file that has been deleted
    view(filename);                                                 //call of view funcion to ensure everything is edited or not
}
void edit(char *filename,char *newcont,char *tagsearch)         //function that searches for the tag to be edited and calls funtion to edit
{   
    FILE *fptr;
    fptr=fopen(filename,"r+");
    if(fptr==NULL)
    {
        fprintf(stderr,"file sample is not opening\n");
        return;
    }
    fseek(fptr,10,SEEK_CUR); int count=0;
    do{
        char tag[5];       
        fread(tag,4,1,fptr);                                    //reads the 4 bytes of the tag
        tag[4]='\0';
        int size=readsize(fptr);
        if(strcmp(tag,tagsearch)==0)
        {
            editingfun(fptr,filename,newcont,size);
            break;
        }
        else
        {       
            fseek(fptr,3,SEEK_CUR);
            fseek(fptr,size,SEEK_CUR);
        }
        count++;
    }while(count<6);
}


int mymain(int ch1,int ch2,char *newcont,char *filename)
{   
    FILE *fp;
    fp=fopen(filename,"r");
    if(fp==NULL)
    {
        fprintf(stderr,"file not found \n");
        return 0;
       
    }
    char idver[5];
    fread(idver,3,1,fp);
    if(strcmp(idver,"ID3")==0)
    {
        printf("Valid ID3 file present \n");
    }
    else
    {
        printf("INVALID file which is not ID3\n");
        return 0;
    }
    char ch;
    fread(&ch,1,1,fp);
    if(ch==3)
        printf("Version is of ID3 VERSION 3\n");
    else{
        printf("Version is not ID3 VERSION 3\n");
        return 0;
    }
    fclose(fp);
    if(ch1=='v')     // to check what is the option entered by the user
    {
        view(filename);                         //funtction call to view
        printf("File view is succesfull \n");
    }
    if(ch1=='e')
    {   
        if(ch2=='t'){
            edit(filename,newcont,"TIT2"); printf("title  has been edited succesfully\n");
        }
        else if(ch2=='a'){
            edit(filename,newcont,"TPE1"); printf("Artist name has been edited succesfully\n");
        }
        else if(ch2=='A'){
            edit(filename,newcont,"TALB"); printf("Album name  has been edited succesfully\n");
        }
        else if(ch2=='y'){
            edit(filename,newcont,"TYER"); printf("Year has been edited succesfully\n");
        }
        else if(ch2=='m'){
            edit(filename,newcont,"TCON"); printf("Music  has been edited succesfully\n");
        }
        else if(ch2=='c'){
            edit(filename,newcont,"TCOM"); printf("comment has been edited succesfully\n");
        }
        else{
            printf("Editing option not entered in range\n");
            exit(1);
        }
    }
}
void invalidarguments()
{
    printf("----------------------------------------------\n");
    printf("ERROR: ./a.out : INVALID ARGUMENTS\n");
    printf("USAGE :\n");
    printf("To view please pass like : ./a.out -v mp3filename\n");
    printf("To edit please pass like : ./a.out -e -t/-A/-a/-m/-y/-c newstring mp3filename\n");
    printf("To get a manual of how to use it please pass like : ./a.out --help\n");
    printf("----------------------------------------------\n");
}
void help()
{
    printf("----------------------------------------------\n");
    printf("To view please pass like : ./a.out -v mp3filename\n");
    printf("To edit please pass like : ./a.out -e -t/-A/-a/-m/-y/-c newstring mp3filename\n");
    printf("$ mp3tag --help\n");
    printf("usage: mp3tag -[tTaAycg] \"value\" file1\n");
    printf("       mp3tag -v\n");
    printf("-t\tModifies a Title tag\n");
    printf("-T\tModifies a Track tag\n");
    printf("-a\tModifies an Artist tag\n");
    printf("-A\tModifies an Album tag\n");
    printf("-y\tModifies a Year tag\n");
    printf("-c\tModifies a Comment tag\n");
    printf("-g\tModifies a Genre tag\n");
    printf("-h\tDisplays this help info\n");
    printf("-v\tPrints version info\n");
    printf("----------------------------------------------\n");
}


