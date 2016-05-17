/*
* Jake Halloran
* Yahtzee implementation for CS 0449 Project 4
* Last Edited: 4/9/16
**/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#define NUM_DICE 5
#define NUM_TURNS 13

int upper_data[6];
int lower_data[7];
int has_bonus;
int roll[5];
int needs_rolled[5];
int score_pos_used[13];
int score;
int dice_values[6];
int top_full;
int bottom_full;

//Takes an array of dice and rolls any dice that has a 1
//At the matching index in the needs_rolled array of equal length
void roll_dice(){
	int i;
	char die[1];
	
	//Open the driver to read from
	FILE * dice_file = fopen("/dev/dice","r");
	
	for(i = 0;i<NUM_DICE;i++){
		if(needs_rolled[i]==1){
			fread(die,1,1,dice_file);
			roll[i] = die[0] + 1;
			needs_rolled[i] = 0;
		}
	}
	fclose(dice_file);
	return;
}

//Comparator for dice for qsort
int int_cmp(const void *a, const void *b){
	return(*(int *)a - *(int *)b);
}

//Gets user reroll choice
int get_dice_to_roll(){
	char line[1000], *p, *e;
	int temp = 0;
	char temp_char;
	
	printf("\nWhich dice to reroll? Enter 0 for none (1-5): ");
	//Uses fgets and string processing to read in an indeterminate number of ints and then mark those dice
	//to be rerolled.
	fgets(line, sizeof(line), stdin);
	p = line;
	for(p = line; ;p = e){
		temp = strtod(p,&e);
		if (p==e){
			break;
		}
		
		//If the user enteres a zero, skip rerolling
		if(temp == 0){
			return 0;
		}
		
		if(temp>0&&temp<6){	
			needs_rolled[temp-1] = 1;
		}
	} 
	
	return 1;
}

//Displays a menu of user places to store the dice score
//Returns the users selection
int display_menu(){
	int user_select = 0;
	
	//Selects point section to display
	printf("Place dice into: \n1) Upper Section\n2) Lower Section\n");
	while(user_select!=1 && user_select !=2){
		printf("\nSelection: ");
		scanf("%d",&user_select);
		if((user_select==1&&top_full==6)||(user_select==2&&bottom_full==7)){
			printf("Sorry, that section is full.\n");
			user_select = 0;
		}
	}
	
	//Upper section that is just the name of the dice values
	if(user_select ==1){
		user_select = 0;
		top_full++;
		printf("\n\nPlace dice into:\n1) Ones\n2) Twos\n3) Threes\n");
		printf("4) Fours\n5) Fives\n6) Sixes\n");
		while(user_select<1|| user_select>6){
			printf("\nSelection (1-6): ");
			scanf("%d",&user_select);
			if(score_pos_used[user_select-1]&&user_select>0&&user_select<7){
				printf("You have already used that score position please seleect again.");
				user_select = 0;
			}
		}
		return user_select;
	}
	
	//Lower section display
	user_select = 0;
	bottom_full++;
	printf("\n\nPlace dice into: \n1) Three of a Kind\n2) Four of a Kind: \n3) Small Straight\n");
	printf("4) Large Straight\n5) Full House\n6) Yahtzee\n7) Chance");
	while(user_select<1||user_select>7){
		printf("\nSelection (1-7): ");
		scanf("%d",&user_select);
		if(score_pos_used[user_select+5]&&user_select>0&&user_select<8){
			printf("You have already used that score position please seleect again.");
			user_select = 0;
		}	
	}
	return(user_select+6);
}

//Stores the user's latest turn wherever they choose
void store_dice(int choice){
	int i =0;
	int j =0;
	int sum = 0;
	
	for(i = 0;i<NUM_DICE;i++){
		sum+=roll[i];
	}
	
	//Upper data section points calculated by multiplying the recureance of the value by the value
	if(choice<7){
		upper_data[choice-1] = dice_values[choice-1]*choice;
		score_pos_used[choice-1] = 1;
		score+=upper_data[choice-1];
		return;
	}
	
	//If user has 3 of a kind store the dice sum otherwise store 0
	if(choice == 7){
		for(i = 0;i<6;i++){
			if(dice_values[i]>=3){
				lower_data[0] = sum;
				score_pos_used[6] = 1;
				score+=sum;
				return;
			}
		}
		lower_data[choice -7] = 0;
		score_pos_used[choice-1] = 1;
		return;
	}
	
	//If user has 4 of a kind store the dice sum otherwise store 0
	else if(choice == 8){
		for(i = 0;i<6;i++){
			if(dice_values[i]>=4){
				lower_data[choice-7] = sum;
				score_pos_used[choice-1] = 1;
				score+=sum;
				return;
			}
		}
		lower_data[choice -7] = 0;
		score_pos_used[choice-1] = 1;
		return;
	}
	
	//If user has a small straight stores 30 otherwise stores 0
	else if(choice == 9){
		if(dice_values[2]>0&&dice_values[3]>0){
			if((dice_values[0]>0&&dice_values[1]>0)||(dice_values[1]>0&&dice_values[4]>0)
				||(dice_values[4]>0&&dice_values[5]>0)){
					lower_data[choice-7] = 30;
					score_pos_used[choice-1] = 1;
					score+=30;
					return;
			}
		}
		lower_data[choice -7] = 0;
		score_pos_used[choice-1] = 1;
		return;
	}
	
	//If user has large straight stores 40 otherwise stores 0
	else if(choice == 10){
		if(dice_values[1]>0&&dice_values[2]>0&&dice_values[3]>0&&dice_values[4]>0){
			if(dice_values[0]>0||dice_values[5]>0){
				lower_data[choice-7] = 40;
				score_pos_used[choice-1] = 1;
				score+=40;
				return;
			}
		}
		lower_data[choice -7] = 0;
		score_pos_used[choice-1] = 1;
		return;
	}
	
	//If user has full house store 25 otherwise store 0
	else if(choice == 11){
		for(i = 0; i<6; i++){
			if(dice_values[i] == 3){
				for(j=0;j<6;j++){
					if(dice_values[j]==2){
						lower_data[choice-7] = 25;
						score_pos_used[choice-1] = 1;
						score+=25;
						return;
					}
				}
			}
		}
		lower_data[choice -7] = 0;
		score_pos_used[choice-1] = 1;
		return;
	}
	
	//If the user has a yahtzee stores 50 otherwise stores 0
	else if(choice == 12){
		for(i=0;i<6;i++){
			if(dice_values[i]==5){
				lower_data[choice-7] = 50;
				score_pos_used[choice-1] = 1;
				score+=50;
				return;
			}
		}
		lower_data[choice-7] = 0;
		score_pos_used[choice-1] = 1;
		return;
	}
	
	//If user has picked chance, just store dice sum
	else if(choice==13){
		lower_data[choice-7] = sum;
		score_pos_used[choice-1] = 1;
		score+=sum;
		return;
	}
}

//Displays the user's score after storing the latest turn
void display_score(){
	int i =0;
	printf("\nYour score: %d\n",score);
	printf("\nOnes: ");
	if(score_pos_used[i]==1) {printf("%d",upper_data[i++]);}
	else {i++;}
	
	printf("\nTwos: ");
	if(score_pos_used[i]==1) {printf("%d",upper_data[i++]);}
	else {i++;}
	
	printf("\nThrees: ");
	if(score_pos_used[i]==1) {printf("%d",upper_data[i++]);}
	else {i++;}
	
	printf("\nFours: ");
	if(score_pos_used[i]==1) {printf("%d",upper_data[i++]);}
	else {i++;}
	
	printf("\nFives: ");
	if(score_pos_used[i]==1) {printf("%d",upper_data[i++]);}
	else {i++;}
	
	printf("\nSixes: ");
	if(score_pos_used[i]==1) {printf("%d",upper_data[i++]);}
	else {i++;}
	
	printf("\nUpper Section Bonus: ");
	if(has_bonus) {printf("%d",(35));}
	else {printf("0");}
	
	printf("\nThree of a Kind: ");
	if(score_pos_used[i]==1) {printf("%d",lower_data[i-6]); i++;}
	else {i++;}
	
	printf("\nFour of a Kind: ");
	if(score_pos_used[i]==1) {printf("%d",lower_data[i-6]); i++;}
	else {i++;}
	
	printf("\nSmall Straight: ");
	if(score_pos_used[i]==1) {printf("%d",lower_data[i-6]); i++;}
	else {i++;}
	
	printf("\nLarge Straight: ");
	if(score_pos_used[i]==1) {printf("%d",lower_data[i-6]); i++;}
	else {i++;}
	
	printf("\nFull House: ");
	if(score_pos_used[i]==1) {printf("%d",lower_data[i-6]); i++;}
	else {i++;}
	
	printf("\nYahtzee: ");
	if(score_pos_used[i]==1) {printf("%d",lower_data[i-6]); i++;}
	else {i++;}
	
	printf("\nChance: ");
	if(score_pos_used[i]==1){ printf("%d",lower_data[i-6]); i++;}
	else {i++;}
}

//Calls the score storing funciton and then the score display funciton
void get_points(){
	int i =0;
	int bonus_sum = 0;
	
	//Resets the dice values
	for(i = 0;i<6;i++){
		dice_values[i]=0;
	}
	
	//Stores the number of times each face was in the roll
	for(i = 0;i<NUM_DICE;i++){
		dice_values[roll[i]-1]++;
	}
	int choice = display_menu();
	store_dice(choice);
	
	//adds the bonus if necessary
	if(!has_bonus){
		for(i=0;i<6;i++){
			bonus_sum += upper_data[i];
		}
		if(bonus_sum>=63){
			score+=35;
			has_bonus=1;
		}
	}
	
	//Displays the current score
	display_score();
	
}

//Represents an individual turn in yahtzee
void play_turn(){
	int roll_count = 0;
	int i =0;
	
	//Initialize dice for turn
	for(i = 0;i<NUM_DICE;i++){
		roll[i]=0;
		needs_rolled[i]=1;
	}
	
	roll_dice(); //initial dice roll
	printf("\nYour roll: \n");
	printf("%d %d %d %d %d\n",roll[0],roll[1],roll[2],roll[3],roll[4]);
	
	//Rerolls desired dice or user enters 0 to skip rerolling
	if(get_dice_to_roll()){
		roll_dice();
		printf("\nYour second roll: \n");
		printf("%d %d %d %d %d\n",roll[0],roll[1],roll[2],roll[3],roll[4]);
		
		//Rolls third and final time if desired
		if(get_dice_to_roll()){
			roll_dice();
			printf("\nYour third roll: \n");
			printf("%d %d %d %d %d\n",roll[0],roll[1],roll[2],roll[3],roll[4]);
		}
	}
	printf("\n");
	qsort(roll,NUM_DICE,sizeof(int),int_cmp); //sorts the roll for ease of processing
	get_points(); //calls subfunction to allow uesr to pick point value of roll
	
}

//Initalizes global variables and then calls the play_turn function 13 times
int main(void){
	srand(time(NULL));
	char play_again = 'y';
	int i = 0;
	char temp_char;
		
	while(play_again=='y'){
		//Reset score and bonus
		score = 0;
		has_bonus = 0;
		
		//Reset the top and bottom full markers
		top_full = 0;
		bottom_full = 0;
		
		//Initialize score holders for game
		lower_data[0] = 0;
		for(i = 0;i<6;i++){
			lower_data[i+1] = upper_data[i] = -1;
		}
		
		//reset score position holder
		for(i =0;i<13;i++) {
			score_pos_used[i] = 0;
		}
		
		//Loop 13 turns
		printf("\nBeginning Game of Yahtzee\n");
		for(i =0; i < NUM_TURNS; i++){
			play_turn();
			printf("\n");
			scanf("%c",&temp_char);
		}
		
		//Allows user to start a new game
		play_again = 'k';		
		while(play_again!='n' && play_again!='y'){
			printf("\nDo you want to play again? (y/n): ");
			scanf("%c",&play_again);
		}
		
	}
	
	return 0;
}
