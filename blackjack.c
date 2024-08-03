/*
IMPORTANT: DOES NOT WORK, SEE README FOR DETAILS

project: 03
author: Hasnain Ali
email: hali6@umbc.edu
student id: jl69013
github username: hali0860
description: You play a game of blackjack with the kernel
*/

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/random.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/string.h>
#include <linux/uaccess.h>

MODULE_LICENSE("GPL");

#define DEVICE_NAME "blackjack"

// Game Conditions
static bool alreadyReset = false;         // If the user entered "Reset"
static bool alreadyShuffled = false;      // If the user entered "Shuffle"
static bool alreadyDealt = false;         // If the user entered "Deal"
static bool inGame = false;               // If the user is already in a game
static bool dealerTurn = false;           // If it's the dealer's turn
static bool afterGame = false;            // If it's after a game

// A struct for cards
struct Card {
    char rank[3];
    char suit[10];
    char display[20];
    long value;
};

static char userInput[1024];              // Used to store user input
struct Card deck[52];                     // An array that has all the cards in a deck
struct Card userDeck[52];                 // An array that has the user's hand
static int userDeckIndex;                 // The number of cards in the user's hand
struct Card dealerDeck[52];               // An array that has the dealer's hand
static int dealerDeckIndex;               // The number of cards in the dealer's hand
int cardsLeft;                            // Tracks the number of cards left in a deck

// Initializes a card with all of it's infomration
void initializeCard(struct Card *card, const char *rank, const char *suit) {
    snprintf(card->rank, sizeof(card->rank), "%s", rank);
    snprintf(card->suit, sizeof(card->suit), "%s", suit);
    snprintf(card->display, sizeof(card->display), "%s of %s", rank, suit);
    // Gets the number value of each card
    if (strcmp(rank, "A") == 0) {
        card->value = 11;
    }
    else if (strcmp(rank, "K") == 0 || strcmp(rank, "Q") == 0 || strcmp(rank, "J") == 0) {
        card->value = 10;
    }
    else {
        card->value = kstrtol(rank, 10, &card->value) ? 0 : card->value;
    }
}

// Initializes 52 cards
void initializeDeck(struct Card *deck, int numCards) {
    static const char *ranks[] = {"2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K", "A"};
    static const char *suits[] = {"Hearts", "Diamonds", "Clubs", "Spades"};
    int index = 0;
    int i, j;
    for (i = 0; i < 13; i++) {
        for (j = 0; j < 4; j++) {
            initializeCard(&deck[index], ranks[i], suits[j]);
            index++;
        }
    }
}

// Draws a card
struct Card drawCard(void) {
    int randomIndex;
    struct Card drawnCard;
    // Gest a random number
    get_random_bytes(&randomIndex, sizeof(randomIndex));
    // Ensures it is in bounds
    randomIndex %= cardsLeft;
    // Saves the card
    drawnCard = deck[randomIndex];
    // Places the drawn card at the end of the array and places the previously last
    // card in the array whee the drawn card used to be
    deck[randomIndex] = deck[cardsLeft - 1];
    // Makes it so that the last card will not be accessible anymore, which is the
    // drawn card
    cardsLeft--;
    return drawnCard;
}

// Code for open operation
static int blackjack_open(struct inode *inode, struct file *file) {
    printk(KERN_INFO "blackjack opened\n");
    return 0;
}

// Code for read operation
static ssize_t blackjack_read(struct file *file, char __user *buffer, size_t length, loff_t *offset) {
    char dealerOutput[1800];
    char scoreString[30];
    int userScore = 0;
    int dealerScore = 0;
    size_t bytesRead;
    size_t index;
    char *output;
    int i;
    // Check if we've already provided a response
    if (*offset > 0) {
        // If yes, signal EOF
        return 0;
    }
    // If the user is not in the middle of a game and has not just finished one
    if ((inGame == false) && (afterGame == false)) {
	// If the user did not enter "Reset" yet
	if (alreadyReset == false) {
	    // If the current user input is not "Reset"
    	    if (strcmp(userInput, "Reset") != 0) {
        	output  = "You must enter \"Reset\" before you can start playing";
		strncpy(dealerOutput, output, sizeof(dealerOutput) - 1);
		dealerOutput[sizeof(dealerOutput) - 1] = '\0';
    	    }
	    // If the current user input is "Reset"
	    else if (strcmp(userInput, "Reset") == 0) {
		output = "OK";
                strncpy(dealerOutput, output, sizeof(dealerOutput) - 1);
                dealerOutput[sizeof(dealerOutput) - 1] = '\0';
		alreadyReset = true;
		// Reset the states to the initial values
		alreadyShuffled = false;
		alreadyDealt = false;
		inGame = false;
		dealerTurn = false;
		afterGame = false;
	    }
	}
	// If the user did not enter "Shuffle" yet
	else if (alreadyShuffled == false) {
            // If the current user input is not "Shuffle"
            if (strcmp(userInput, "Shuffle") != 0) {
                output = "You must enter \"Shuffle\" before you can start playing";
                strncpy(dealerOutput, output, sizeof(dealerOutput) - 1);
                dealerOutput[sizeof(dealerOutput) - 1] = '\0';
            }
            // If the current user input is "Shuffle"
            else if (strcmp(userInput, "Shuffle") == 0) {
                output = "OK";
                strncpy(dealerOutput, output, sizeof(dealerOutput) - 1);
                dealerOutput[sizeof(dealerOutput) - 1] = '\0';
                alreadyShuffled = true;
            }
	}
	// If the user did not enter "Deal" yet
	else if (alreadyDealt == false) {
            // If the current user input is not "Deal"
            if (strcmp(userInput, "Deal") != 0) {
                output = "You must enter \"Deal\" before you can start playing";
                strncpy(dealerOutput, output, sizeof(dealerOutput) - 1);
                dealerOutput[sizeof(dealerOutput) - 1] = '\0';
            }
            // If the current user input is "Deal"
            else if (strcmp(userInput, "Deal") == 0) {
                alreadyDealt = true;
		inGame = true;
		// Drawing two cards
		userDeckIndex = 0;
		for (i = 0; i < 2; i++) {
    		    // Draw a card
    		    userDeck[userDeckIndex] = drawCard();
		    // Used for debugging
		    printk(KERN_INFO "userDeckIndex is %d", userDeckIndex);
		    printk(KERN_INFO "%s", userDeck[userDeckIndex].display);
    		    // Increment the userDeckIndex
    		    userDeckIndex++;
		}
                strncpy(dealerOutput, "OK\nPlayer has ", sizeof(dealerOutput) - 1);
		for (i = 0; i < userDeckIndex; ++i) {
		    userScore = userScore + userDeck[i].value;
    		    // Concatenate the card display to dealerOutput
    		    strncat(dealerOutput, userDeck[i].display, sizeof(dealerOutput) - strlen(dealerOutput) - 1);
    		    // Add " and " if it's not the last card
    		    if (i < userDeckIndex - 1) {
        	        strncat(dealerOutput, " and ", sizeof(dealerOutput) - strlen(dealerOutput) - 1);
    		    }
		}
		snprintf(scoreString, sizeof(scoreString), " for a total of %d\n", userScore);
                strncat(dealerOutput, scoreString, sizeof(dealerOutput) - strlen(dealerOutput) - 1);
		if (userScore > 21) {
		    // Player is over 21, dealer wins
    		    strncat(dealerOutput, "Player is over 21.\nDealer is the winner.\n", sizeof(dealerOutput) - strlen(dealerOutput) - 1);
		    strncat(dealerOutput, "Do you wish to play with the same deck?", sizeof(dealerOutput) - strlen(dealerOutput) - 1);
		    inGame = false;
		    afterGame = true;
		}
		else {
    		    // Player is not over 21
    		    strncat(dealerOutput, "Does player want another card? If so, respond with \"Hit\" or \"No\" for hold.", sizeof(dealerOutput) - strlen(dealerOutput) - 1);
		}
		dealerOutput[sizeof(dealerOutput) - 1] = '\0';
            }
        }
    }
    // If the user is in a game
    else if (inGame == true) {
	// If the user entered "Hit"
        if ((strcmp(userInput, "Hit") == 0)) {
	    userDeck[userDeckIndex] = drawCard();
            // Increment the userDeckIndex
            userDeckIndex++;
            strncpy(dealerOutput, "OK\nPlayer has ", sizeof(dealerOutput) - 1);
            for (i = 0; i < userDeckIndex; ++i) {
                userScore = userScore + userDeck[i].value;
                // Concatenate the card display to dealerOutput
                strncat(dealerOutput, userDeck[i].display, sizeof(dealerOutput)- 1);
                // Add " and " if it's not the last card
                if (i < userDeckIndex - 1) {
                    strncat(dealerOutput, " and ", sizeof(dealerOutput) - strlen(dealerOutput) - 1);
                }
            }
            snprintf(scoreString, sizeof(scoreString), " for a total of %d\n", userScore);
            strncat(dealerOutput, scoreString, sizeof(dealerOutput) - strlen(dealerOutput) - 1);
            if (userScore > 21) {
                // Player is over 21, dealer wins
                strncat(dealerOutput, "Player is over 21.\nDealer is the winner.\n", sizeof(dealerOutput) - strlen(dealerOutput) - 1);
		strncat(dealerOutput, "Do you wish to play with the same deck?", sizeof(dealerOutput) - strlen(dealerOutput) - 1);
                inGame = false;
                afterGame = true;
            }
            else {
                // Player is not over 21
                strncat(dealerOutput, "Does player want another card? If so, respond with \"Hit\" or \"No\" for hold.", sizeof(dealerOutput) - strlen(dealerOutput) - 1);
            }
            dealerOutput[sizeof(dealerOutput) - 1] = '\0';
	}
        // If the user entered "No"
	else if (strcmp(userInput, "No") == 0) {
     	    dealerTurn = true;
	    for (i = 0; i < userDeckIndex; ++i) {
                userScore = userScore + userDeck[i].value;
	    }
	    strncpy(dealerOutput, "Player has a total of %d\n", userScore);
	}
	// If the user did not enter "Hit" or "No"
	else {
	    output = "Please enter \"Hit\" or \"No\"";
            strncpy(dealerOutput, output, sizeof(dealerOutput) - 1);
            dealerOutput[sizeof(dealerOutput) - 1] = '\0';
	}
	// If it is the delaer's turn
	if (dealerTurn == true) {
	    dealerDeckIndex = 0;
            for (i = 0; i < 2; i++) {
                // Draw a card
                dealerDeck[dealerDeckIndex] = drawCard();
                // Increment the dealerDeckIndex
                dealerDeckIndex++;
            }
            strncpy(dealerOutput, "Dealer has ", sizeof(dealerOutput) - 1);
            for (i = 0; i < dealerDeckIndex; ++i) {
                dealerScore = dealerScore + dealerDeck[i].value;
                // Concatenate the card display to dealerOutput
                strncat(dealerOutput, dealerDeck[i].display, sizeof(dealerOutput)- 1);
                // Add " and " if it's not the last card
                if (i < dealerDeckIndex - 1) {
                    strncat(dealerOutput, " and ", sizeof(dealerOutput) - strlen(dealerOutput) -1);
                }
            }
            snprintf(scoreString, sizeof(scoreString), " for a total of %d\n", dealerScore);
            strncat(dealerOutput, scoreString, sizeof(dealerOutput) - strlen(dealerOutput) -1);
            if (dealerScore > 21) {
                // Dealer is over 21, Player wins
                strncat(dealerOutput, "Dealer is over 21.\nPlayer is the winner.\n", sizeof(dealerOutput) - strlen(dealerOutput) - 1);
		strncat(dealerOutput, "Do you wish to play with the same deck?", sizeof(dealerOutput) - strlen(dealerOutput) - 1);
		dealerOutput[sizeof(dealerOutput) - 1] = '\0';
                inGame = false;
                afterGame = true;
            }
            else {
		while (dealerScore < 17) {
		    dealerScore = 0;
		    dealerDeck[dealerDeckIndex] = drawCard();
                    // Increment the dealerDeckIndex
                    dealerDeckIndex++;
                    strncpy(dealerOutput, "Dealer has ", sizeof(dealerOutput) - 1);
                    for (i = 0; i < dealerDeckIndex; ++i) {
                	dealerScore = dealerScore + dealerDeck[i].value;
                	// Concatenate the card display to dealerOutput
                	strncat(dealerOutput, dealerDeck[i].display, sizeof(dealerOutput)- 1);
                	// Add " and " if it's not the last card
                	if (i < dealerDeckIndex - 1) {
                    	    strncat(dealerOutput, " and ", sizeof(dealerOutput) - strlen(dealerOutput) -1);
                	}
            	    }
            	    snprintf(scoreString, sizeof(scoreString), " for a total of %d\n", dealerScore);
            	    strncat(dealerOutput, scoreString, sizeof(dealerOutput) - strlen(dealerOutput) -1);
            	    if (dealerScore > 21) {
                	// Dealer is over 21, Player wins
                	strncat(dealerOutput, "Dealer is over 21.\nPlayer is the winner.\n", sizeof(dealerOutput) - strlen(dealerOutput) - 1);
			strncat(dealerOutput, "Do you wish to play with the same deck?", sizeof(dealerOutput) - strlen(dealerOutput) - 1);
			dealerOutput[sizeof(dealerOutput) - 1] = '\0';
                	inGame = false;
                	afterGame = true;
            	    }
		}
		// If both dealer and player did not go over 21
		if (dealerScore <= 21) {
		    // Compare the score of the dealer to the player
		    for (i = 0; i < userDeckIndex; ++i) {
                        userScore = userScore + userDeck[i].value;
		    }
	  	    if (dealerScore >= userScore) {
		        strncat(dealerOutput, "Dealer is the winner.\n", sizeof(dealerOutput) - strlen(dealerOutput) - 1);
		        strncat(dealerOutput, "Do you wish to play with the same deck?", sizeof(dealerOutput) - strlen(dealerOutput) - 1);
		        dealerOutput[sizeof(dealerOutput) - 1] = '\0';
		    }
		    else {
		        strncat(dealerOutput, "Player is the winner.\n", sizeof(dealerOutput) - strlen(dealerOutput) - 1);
		        strncat(dealerOutput, "Do you wish to play with the same deck?", sizeof(dealerOutput) - strlen(dealerOutput) - 1);
		        dealerOutput[sizeof(dealerOutput) - 1] = '\0';
		    }
	        }
	    }
        }
    }
    // If the game is finished
    else if ((inGame == false) && (afterGame == true)) {
	// If the user entered "Yes" to playing with the same deck
	if (strcmp(userInput, "Yes") == 0) {
	    // keep cardsleft to it's current value
	    afterGame = false;
	    alreadyReset = false;
	}
	// If the user entered "No" to playing with the same deck
	else if (strcmp(userInput, "No") == 0) {
	    // reset cardsleft to 52
	    cardsLeft = 52;
	    afterGame = false;
	    alreadyReset = false;
	}
	// If the user entered invalid input
	else {
	    output = "Please enter \"Yes\" or \"No\" to keep playing";
            strncpy(dealerOutput, output, sizeof(dealerOutput) - 1);
            dealerOutput[sizeof(dealerOutput) - 1] = '\0';
	}
    }
    // Copy the phrase to the user buffer
    bytesRead = 0;
    index = 0;
    while (length && dealerOutput[index] != '\0') {
        put_user(dealerOutput[index], buffer++);
        index++;
        bytesRead++;
        length--;
    }
    // Add a newline character at the end of the response for better readability
    if (bytesRead > 0) {
        put_user('\n', buffer++);
        bytesRead++;
    }
    // Update the offset to signal that we've provided a response
    *offset += bytesRead;
    return bytesRead;
}

// Code for write operation
static ssize_t blackjack_write(struct file *file, const char __user *buffer, size_t length, loff_t *offset) {
    // If the user input exceeds the set buffer for userInput (1024)
    if (length > sizeof(userInput) - 1) {
        printk(KERN_ERR "User input exceeds buffer size\n");
        return -EINVAL;
    }
    // If there is an error when copying from user space
    if (copy_from_user(userInput, buffer, length)) {
        return -EFAULT;
    }
    // Remove newline character if present
    if (userInput[length - 1] == '\n') {
        userInput[length - 1] = '\0';
    }
    else {
        userInput[length] = '\0';
    }
    printk(KERN_INFO "Received input: %s\n", userInput);
    return length;  // Return the number of bytes written
}

// Code for relase operation
static int blackjack_release(struct inode *inode, struct file *file) {
    printk(KERN_INFO "blackjack released\n");
    return 0;
}

// Code for the file operations
static struct file_operations blackjack_fops = {
    .owner = THIS_MODULE,
    .open = blackjack_open,
    .read = blackjack_read,
    .write = blackjack_write,
    .release = blackjack_release,
};

// Misc device structure code
// Sets the read and write only permissions
static struct miscdevice blackjack_misc_device = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = DEVICE_NAME,
    .fops = &blackjack_fops,
    .mode = 0666,
};

// Code for loading the kernel
static int __init blackjack_init(void) {
    int returnValue;
    // Register the misc device
    returnValue = misc_register(&blackjack_misc_device);
    if (returnValue) {
        printk(KERN_ERR "Failed to register misc device\n");
        return returnValue;
    }
    printk(KERN_INFO "blackjack successfully loaded\n");
    initializeDeck(deck, 52);
    cardsLeft = 52;
    return 0;
}

// Code for unloading the kernel
static void __exit blackjack_exit(void) {
    userDeckIndex = 0;
    // Deregister the misc device
    misc_deregister(&blackjack_misc_device);
    printk(KERN_INFO "unloading blackjack\n");
}

module_init(blackjack_init);
module_exit(blackjack_exit);
