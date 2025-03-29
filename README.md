This is a C++ program to compute the US Entry impact of a series of events in the game World in Flames.

Note that this was hacked together in a day and I am publishing it now to show my work for a discussion on Discord.
I make no promises about continuing this work.

You should be able to build the program using (this isn't well tested yet - I've been doing it all from CLion!)
```aiignore
cmake
make
```

Then you should be able to run it by running a command like (unix-like system):
cat events.txt | cmake-build-debug/chits > output.txt

This will give you a big data file with the values for each of 10000 trials at the end of the US Entry step of each turn.

The file format of events.txt is a CSV file with the following fields in order:

| field | description |
| ----- | ----------- |
|year | the full year like 1939|
|turn | the turn number like 5 for Sep/Oct.|
|pool | one of USEGe, USEJa, PactGe, PactJa, PactRu|
|impact | as the usual US Entry Action/Option costs.  Negative for taking away, positive for adding.  Use 10 to definitely draw a chit (e.g. setup).|
|description | any text you like to describe why the chit happened.|
