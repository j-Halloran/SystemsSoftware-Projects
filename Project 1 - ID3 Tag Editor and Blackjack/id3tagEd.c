//Jake Halloran
//CS 0449 Assignment 1
//ID3 v1.1 Tag Editor
//Last Edited: 2/2/16

//Including all libraries needed for string processing
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct id3tag{
	char identifier[3];
	char title[30];
	char artist[30];
	char album[30];
	char year[4];
	char comment[28];
	char separator;
	char track[1];
	char genre[1];
};

int main(int argc, char *argv[]){
	//structure to hold tag data when read and store user input in array form
	struct id3tag tag;
	
	//Fields to hold argv values for readability reasons
	char *userInput;
	char *userField;
	
	//Loop and length ints
	int i =0;
	int j =0;
	int k =0;
	
	//Stops anything from happening if the number of arguments is wrong
	if(argc%2!=0){
		printf("\nThe nummber of command line arguments is invalid.\n");
	}
	
	//Selection to print current file's tag
	else if(argc==2){
		//open in read/update binary mode
		FILE *file = fopen(argv[1], "r+b");
		
		//Only do anything if the file inputted exists
		if (file==NULL){
			printf("The file inputted does not exist.\n");
		}
		else{
			//Seeks the filepointer to 128 bytes before the file's end and read's the last 128 bytes into the tag struct
			fseek(file,-128,SEEK_END);
			fread(&tag,128,1,file);
			
			//If a tag exists, goes through and prints the tag
			if(!strncmp(tag.identifier,"TAG",3)){
				
				//Print Song Title
				printf("\nTitle:\t %.30s\n",tag.title);

				//Print Artist
				printf("Artist:\t %.30s\n",tag.artist);
				
				//Print Album
				printf("Album:\t %.30s\n",tag.album);
				
				//Print Year
				printf("Year:\t %.4s\n",tag.year);

				//Print Comment
				printf("Comment: %.28s\n",tag.comment);

				//Print Track Number
				printf("Track:\t %.1s\n",tag.track);

				//print genre
				printf("Genre:\t %.1s\n",tag.genre);
			}
			
			//If the tag marker is not present, the program says so and exits
			else{
				printf("\nThe entered file does not have an attached ID3 v1.1 tag.\n");
			}
			
			//Close the file to keep everyone happy
			fclose(file);
		}
	}
	
	//Tag changing mode
	else {
		//Update binary mode to read info from end into array
		FILE *file = fopen(argv[1],"r+b");
		fseek(file,-128,SEEK_END);
		fread(&tag,128,1,file);
		
		//Only continues if file exists
		if(file!=NULL){
			
			//If the file already has a tag, just positions at the start of tag data for writing
			if(!strncmp(tag.identifier,"TAG",3)){
				fseek(file,-128,SEEK_END);
			}
			//If tag does not exist reopens file in append binary mode
			else{
				freopen(argv[1],"a+b",file);
				strncpy(tag.identifier,"TAG",128);
				fwrite(&tag,128,1,file);
			}
			
			for(k = 2;k<argc;k+=2){
				userField = argv[k];
				userInput = argv[k+1];
				//Captures the length of the new tag just to make sure not too long
				j= strlen(argv[k+1]);
				if(j<31){				
					//Changes title data
					if(!strncmp(userField,"title",5)||!strncmp(userField,"-title",6)){
						strncpy(tag.title,userInput,30);
					}
				
					//Changes artist data
					else if(!strncmp(userField,"artist",6)||!strncmp(userField,"-artist",7)){
						strncpy(tag.artist,userInput,30);
					}
				
					//Changes album data
					else if(!strncmp(userField,"album",5)||!strncmp(userField,"-album",6)){
						strncpy(tag.album,userInput,30);
					}
				
					//Changes year data
					else if(!strncmp(userField,"year",4)||!strncmp(userField,"-year",5)){
						strncpy(tag.year,userInput,4);
					}
				
					//Changes comment data
					else if(!strncmp(userField,"comment",7)||!strncmp(userField,"-comment",8)){
						strncpy(tag.comment,userInput,28);
					}
				
					//Changes track data
					else if(!strncmp(userField,"track",5)||!strncmp(userField,"-track",6)){
						strncpy(tag.track,userInput,1);
					}	

					//Tells user if their field value is invalid
					else{
						printf("\nAn entered field is invalid, all other fields will still be edited.\n");
					}
				
				}
			
				//Tells user if they entered too much data
				else{
					printf("\nAt least one value inputted is too long.\n");
				}
			}
			//Writes tag to file and closes the file
			fwrite(&tag,128,1,file);
			fclose(file);
		}
		
		//Exits if file inputted doees not exist
		else{
			printf("\nThe inputted file does not exist.\n");
		}
	}
}