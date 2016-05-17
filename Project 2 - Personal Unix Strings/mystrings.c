//Name: Jake Halloran
//CS 449: Project 2
//Finds and displays all strings of four or more characters

#include<stdlib.h>
#include<stdio.h>
#include<string.h>

int main(int argc, char * argv[]){
	FILE *f; //input file pointer
	long printable_chars =0; //count of characters to print
	char * printable_string; //String of at least 4 valid ascii characters
	char next_char = 0; //current char being read
	long file_size = 0; //finds the size in bytes of the file
	long count; //tracks the remaining length of the file

	//Makes sure the user passes in a filename and exits if not
	if(argc != 2){
		printf("Invalid console arguments.\n");
		return(1);
	}
	
	//Opens the file in read binary mode for ease of use across filetypes
	f = fopen(argv[1], "rb");
	
	//Ends the program if the file doesn't exist or is not readable
	if(f==NULL){
		printf("The file does not exist or there was an error opening it.\n");
		return (1);
	}
	
	//Finds the length in bytes of the file and stores it
	fseek(f,0,SEEK_END);
	count = file_size = ftell(f);
	rewind(f);
	
	//Malloc's space for the printable string plus 1 for null terminator
	printable_string = (char *) malloc(sizeof(char)*(file_size+1));
	
	while(count>0){
		//Reads 1 byte from the file and stores it as the next character
		fread(&next_char,1,1,f);
		
		//If the next character is a printable basic ascii character, it is added to the string
		if(next_char < 127 && next_char > 31){
			printable_string[printable_chars] = next_char;
			printable_chars++;
		} 
		
		//runs if an unprintable character is read
		else{
			//Prints the string if at least the previous 4 characters were valid
			if(printable_chars > 3){
				printable_string[printable_chars] = 0;
				printf("%s\n",printable_string);
			}
			
			//Resets character count and therefore the string
			printable_chars = 0;
		}
		//Decrements the number of bytes remaining to read
		count -= 1;
	}
	//Frees the string's memory before exiting
	free(printable_string);
	
	return(0);
}