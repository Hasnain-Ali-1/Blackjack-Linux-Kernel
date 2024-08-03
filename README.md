For reference, when I say enter "make" or any other command, I mean to enter it in 
your terminal without the quotes, so make.

// WARNING //
In my code, I created a card struct and I make 52 of them and place them in an array.
Then when I draw cards, I'd randomly select one and place it in either the userDeck 
array or the dealerDeck array, depending on who's turn it is. However, when I go to 
draw the cards, sometimes the cards get added to the arrays, and sometimes only a 
space gets added. I printed the cards that would get selected to the kernel, and it
also showed that the cards would sometimes just be spaces. I tried looking to see if 
I was going out of bounds when drawing cards, but I could not see anything wrong with
the code. I must keep missing the error. Because of this, I wasn't able to properly 
test and debug my code, so some of it may not work. Below is my kernel log that shows 
what I am talking about in reference to the cards being spaces:

[57661.169266] userDeckIndex is 0

[57661.169268] userDeckIndex is 1
[57661.169268] A of Diamonds

In this scenario, at the start of the game I was given two cards, those two cards were
the " " and "A of Diamonds". Where that blank spot is should be my first card, but it
is not. This is from the sample output from the game played at the bottom of the 
README, so you can see how this error looks like in an actual game.

// Important //
I don't have the ace functionality implemented where it becomes a 1 instead of an 11 if
the current player goes over 21

// What It Does //
The kernel module simulates a dealer playing a game of blackjack

// How To Run //
To compile, enter "make". To load the module, type "sudo insmod blackjack.ko". To write
to the driver, you need type something like "echo "Reset" > /dev/blackjack". To get 
the response from said echo command, you would type "cat /dev/blackjack". To unload 
the module, type "sudo rmmod blackjack.ko". To  clean up and remove executables in the
directory, "make clean" can be entered. To run the project, you need to "Reset", then 
"Shuffle", then "Deal", using the echo format I showed previously in this section. 
Also, it is importnat that after each echo command you enter, you also enter the
cat command I mentioned earlier in this section. The game will not progress if you 
don't do this.

// Sample Output //
vboxuser@DebianCMSC421:/usr/src/project3/part2$ sudo insmod blackjack.ko
[sudo] password for vboxuser: 
vboxuser@DebianCMSC421:/usr/src/project3/part2$ echo "Reset" > /dev/blackjack
vboxuser@DebianCMSC421:/usr/src/project3/part2$ cat /dev/blackjack
OK
vboxuser@DebianCMSC421:/usr/src/project3/part2$ echo "Shuffle" > /dev/blackjack
vboxuser@DebianCMSC421:/usr/src/project3/part2$ cat /dev/blackjack
OK
vboxuser@DebianCMSC421:/usr/src/project3/part2$ echo "Deal" > /dev/blackjack
vboxuser@DebianCMSC421:/usr/src/project3/part2$ cat /dev/blackjack
OK
Player has  and A of Diamonds for a total of 11
Does player want another card? If so, respond with "Hit" or "No" for hold.
vboxuser@DebianCMSC421:/usr/src/project3/part2$ echo "Hit" > /dev/blackjack
vboxuser@DebianCMSC421:/usr/src/project3/part2$ cat /dev/blackjack
OK
Player has  and A of Diamonds and  for a total of 11
Does player want another card? If so, respond with "Hit" or "No" for hold.
vboxuser@DebianCMSC421:/usr/src/project3/part2$ echo "Hit" > /dev/blackjack
vboxuser@DebianCMSC421:/usr/src/project3/part2$ cat /dev/blackjack
OK
Player has  and A of Diamonds and  and  for a total of 11
Does player want another card? If so, respond with "Hit" or "No" for hold.
vboxuser@DebianCMSC421:/usr/src/project3/part2$ echo "Hit" > /dev/blackjack
vboxuser@DebianCMSC421:/usr/src/project3/part2$ cat /dev/blackjack
OK
Player has  and A of Diamonds and  and  and  for a total of 11
Does player want another card? If so, respond with "Hit" or "No" for hold.
vboxuser@DebianCMSC421:/usr/src/project3/part2$ echo "Hit" > /dev/blackjack
vboxuser@DebianCMSC421:/usr/src/project3/part2$ cat /dev/blackjack
OK
Player has  and A of Diamonds and  and  and  and  for a total of 11
Does player want another card? If so, respond with "Hit" or "No" for hold.
vboxuser@DebianCMSC421:/usr/src/project3/part2$ echo "Hit" > /dev/blackjack
vboxuser@DebianCMSC421:/usr/src/project3/part2$ cat /dev/blackjack
OK
Player has  and A of Diamonds and  and  and  and  and  for a total of 11
Does player want another card? If so, respond with "Hit" or "No" for hold.
vboxuser@DebianCMSC421:/usr/src/project3/part2$ echo "Hit" > /dev/blackjack
vboxuser@DebianCMSC421:/usr/src/project3/part2$ cat /dev/blackjack
OK
Player has  and A of Diamonds and  and  and  and  and  and  for a total of 11
Does player want another card? If so, respond with "Hit" or "No" for hold.
vboxuser@DebianCMSC421:/usr/src/project3/part2$ echo "Hit" > /dev/blackjack
vboxuser@DebianCMSC421:/usr/src/project3/part2$ cat /dev/blackjack
OK
Player has  and A of Diamonds and  and  and  and  and  and  and 5 of Spades for a total of 16
Does player want another card? If so, respond with "Hit" or "No" for hold.
vboxuser@DebianCMSC421:/usr/src/project3/part2$ echo "No" > /dev/blackjack
vboxuser@DebianCMSC421:/usr/src/project3/part2$ cat /dev/blackjack
Dealer has  and 9 of Hearts and  and 3 of Hearts and  and 8 of Diamonds for a total of
 20
