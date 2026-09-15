/*
Name : Shaun Varghese

date : 15/09/2026

description : MP3 TAG READER AND EDITER

Objective: To develop a command-line application capable of reading and extracting metadata from .mp3 audio files 
using the ID3v2.3 tagging standard.

Project Description:	The MP3 Tag Reader accepts a .mp3 file as input and parses its header and metadata frames.
It then extracts and displays information such as the title, artist, album, and year from the file. 
The application reads the file in binary mode and processes the header fields according to the ID3v2.3 specification.

*/
#include<stdio.h>
#include <string.h>
#include <stdint.h>
#include<stdlib.h>
#include<unistd.h>
#include "mp3tagreader.h"
char filename[100],newcont[100];
char ch1,ch2;
int main(int argc,char **argv)
{   
    if(argc<=1)                                         //to check if the argument is less than or equal to 1
    {
        printf("Error handling\n");
        invalidarguments();
        return 0;
    }
    if(strcmp(argv[1],"-v")==0)                          //if function view is selected as ./a.out -v 
    {   
       if(argc>3) {                                       //if the command line argument has more arguments than 3 
        invalidarguments();                              //it calls the error handling 
        exit(1);
    }    
        ch1=argv[1][1];
        strcpy(filename,argv[2]);                        //storing of data to the variables and strings
        mymain(ch1,ch2,newcont,filename);
    }
    else if(strcmp(argv[1],"-e")==0)
    {
        if(argc!=5) {
            invalidarguments();
            exit(1);
        }
        ch1=argv[1][1];
        ch2=argv[2][1];
        strcpy(newcont,argv[3]);                        //storing of data to the variables and strings
        strcpy(filename,argv[4]);
        if(ch2=='t'||ch2=='a'||ch2=='A'||ch2=='y'||ch2=='m'||ch2=='c')      //checks whether the passed characterd is within the range
        {   
            mymain(ch1,ch2,newcont,filename);           //call of function to edit and view
        }
        else{
            invalidarguments();
            exit(1);
        }
    }
    else if(strcmp(argv[1],"--help")==0)
    {
        printf("help\n");
        help();
    }
    else{
        printf("Error handling\n");
        invalidarguments();
    }
}