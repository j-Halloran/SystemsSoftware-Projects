#include <stdio.h>

int getCard(void);

int main(void){
	//Seed RNG
	srand((unsigned int)time(NULL));
	
	//List of variables used to make c89 happy
	int dealerCount =0;
	int playerCount =0;
	int playerAces =0;
	int dealerAces =0;
	int playerHand[10];
	int dealerHand[10];
	char userInput[10];
	int playerCards =0;
	int dealerCards =0;
	int i=0;
	int j=0;	
	int newCard =0;
	
	//Set initial hands
	newCard = getCard();
	if(newCard == 1){
		playerAces++;
		newCard+=10;
	}
	playerHand[0]=newCard;
	newCard = getCard();
	if(newCard == 1){
		playerAces++;
		newCard+=10;
	}
	playerHand[1]=newCard;
	
	newCard = getCard();
	if(newCard == 1){
		dealerAces++;
		newCard+=10;
	}
	dealerHand[0]=newCard;
	newCard = getCard();
	if(newCard == 1){
		dealerAces++;
		newCard+=10;
	}
	dealerHand[1]=newCard;
	
	//Fill initial hand info
	playerCards = 2;
	playerCount = playerHand[0]+playerHand[1];
	dealerCards = 2;
	dealerCount = dealerHand[0]+dealerHand[1];
	
	//Display hands to give user initial choice
	printf("\n\nDealer Hand:\n ? + %d\n",dealerHand[1]);
	printf("\nYour Hand:\n%d + %d = %d",playerHand[0],playerHand[1],playerCount);
	
	//only gives user hit stand option if they dont have blackjack
	if(playerCount!=21){
		printf("\n\nWould you like to \"hit\" or \"stand\": ");
		fgets(userInput,5,stdin);
		
		//lazy way to continue adding cards, continues iff user tipes exactly 'hit'
		while (playerCount<21&& userInput[0]=='h' && userInput[1]=='i' && userInput[2]=='t'){
			newCard = getCard();
			if(newCard == 1){
				playerAces++;
				newCard+=10;
			}
			playerHand[playerCards] = newCard;
			playerCount+=newCard;
			playerCards++;
			
			//replaces aces of value 11 with value 1 if the user would otherwise bust
			while(playerCount > 21 && playerAces > 0){
				playerAces--;
				for(j = 0;j<playerCards;j++){
					if(playerHand[j]==11){
						playerHand[j]=1;
						playerCount = playerCount -10;
						break;
					}
				}
			}
			
			//Prints user cards in same format as example solution
			printf("\nYour hand: \n");
			for(i=0;i<playerCards;i++){
				printf(" %d ",playerHand[i]);
				if(i!=playerCards-1){
					printf("+");
				}
			}
			
			//asks if the user wants to hit again
			printf(" = %d\n",playerCount);
			if(playerCount<21){	
				printf("\nWould you like to \"hit\" or \"stand\": ");
				fgets(userInput,5,stdin);
			}
		}	
	}
	
	//if initial hand is blackjack, jumps to dealer hand evaluation
	else{
		printf("\n21! You automatically stand.\n");
	}
	
	//only does work for dealer hand if user does not bust
	if(playerCount<=21){
		
		//reveals the secret part of dealer hand
		printf("\nThe dealer:\n%d + %d = %d\n",dealerHand[0],dealerHand[1],dealerCount);
		
		//adds cards to dealer while he has les than 17
		while(dealerCount<17){
			printf("\nThe dealer has less than 17 so he hits.\n");
			newCard = getCard();
			if(newCard ==1){
				dealerAces++;
				newCard+=10;
			}
			dealerHand[dealerCards]=newCard;
			dealerCards++;
			dealerCount+=newCard;
			
			//flips dealer aces from 11 to 1 if necessary to avoid busting
			while(dealerCount>21 && dealerAces>0){
				dealerAces--;
				for(j = 0;j<dealerCards;j++){
					if(dealerHand[j]==11){
						dealerHand[j]=1;
						dealerCount = dealerCount-10;
						break;
					}
				}
			}
			
			//prints dealer hand to match example
			printf("\nThe dealer: \n");
			for (i=0;i<dealerCards;i++){
				printf("%d ",dealerHand[i]);
				if(i!=dealerCards-1){
					printf("+ ");
				}
			}
			printf(" = %d",dealerCount);
		}
		
		
		//Prints final score and who won
		printf("\n\nFinal Totals: \nYou: %d\tDealer: %d\n",playerCount,dealerCount);
		if(dealerCount>21&&playerCount<22){
			printf("\nThe dealer busted. You win!\n");
		}
		else if(dealerCount>playerCount&&dealerCount<22){
			printf("\nThe dealers total is higher, you lose!\n");
		}
		else if(playerCount>dealerCount&&playerCount<22){
			printf("\nYour total is higher, you win!\n");
		}
		else if(playerCount==dealerCount){
			printf("You and the dealer tied. Push!\n");
		}
	}
	
	//else that tells the user they lose if they busted.
	else{
		printf("\n\nYour total is %d: You bust! The dealer wins.\n",playerCount);
	}
	

//returns a random int from 1 to 13 (because of face cards), values greater
//than 13 are converted to their value of 10 and aces are passed as 1
int getCard (void){
	int card = rand()%(13-1+1)+1;
	if (card >9){
		card =10;
	}
	return card;
}