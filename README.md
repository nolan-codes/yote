# Yoté

According to the first paragraph of its Wikipedia Page: 

> Yoté is a traditional strategy board game of West Africa, where it is a popular gambling game due to its fast pace and surprising turnarounds.

[Here's a link to the Wikipedia page, I am building this project entirely from it.](https://en.wikipedia.org/wiki/Yot%C3%A9)

---

I aim to make a console-based interaction system for the game, along with a bot which will use a similar system to chess engines. 

I chose yote because the rules are simple but unique, and I could not find any other documentation or existing engines. As far as I know, this project is the first ever yote engine. I may make a website for playing against the bot later on. 

--- 

Current State of things:

- [x] 5x6 Bitboard representation and basic game values
- [x] Ability to print the board nicely to the console
- [x] Move generation
- [x] Get the current state of the game
- [x] Apply/undo move functionality
- [x] Save/load board states
- [ ] Human interaction system & CLI
- [ ] Board state scoring function
- [ ] Recursive evaluation bot
- [ ] Human + bot CLI

---

To run the project, build it with the Makefile:

```
make
```

Then run the produced executable:

```
./yote
```

Or, if that's too complex, you can also just run the bash script and it'll do both commands:

```
./run.sh
```
