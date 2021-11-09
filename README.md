The program generate tweets from a given file by markov chain model. 
First, the program build a dictionary (linked list), when for each word there is a an array that save all the following words of it, 
and the amount of apperances after the word,for each of them.
After that the program generate the first word in a uniform distribution from all the words that not end the sentence (end with dot), 
and then the next words from the array of following words.


==program arguments===
The program recive 4 arguments
seed - number for srand function
number of tweets - how many tweets we want to generate
path - path to the file to read from
how much to read - how many words to read from the file. in the user wont assert this argument, the program will read all the file

==program files==
twwetsGenerator.c
justdoit_tweets.txt - example file to read from

===how to compile===
gcc -std=c99 tweetsGenerator.c -o tweetsGenerator

==how to run===
.tweetsGenerator seed number of tweets path how much to read (optional)

