#! /bin/bash
# TODO utilize CMDLINE arguments for I/O files
program=wordsearch

clear

echo "Executing make clean..."
make clean

sleep 0.5

echo "Executing make..."
make

if [ $? -eq 0 ]
then
	echo "Make build succeeded."
	echo "Launching $program .. GLHF"
  
	sleep 0.4
	clear

	./$program data/data1 take
else
	echo "Make failed. start.sh will now stop."
fi
