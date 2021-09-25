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
  
  echo "Creating directory ./attempts (if not exists)"
  mkdir -p attempts

  echo "Executing program for each data set provided."
	./$program data/data1 attempts/attempt1.txt
  ./$program data/data2 attempts/attempt2.txt
  ./$program data/data3 attempts/attempt3.txt
  echo "Execution series complete."

  echo "- ** Displaying diffs **"

  echo "- DIFF 1 -"
  diff attempt1 solutions/solution1
  echo "- END DIFF -"
  echo "- DIFF 2 -"
  diff attempt2 solutions/solution2
  echo "- END DIFF -"
  echo "- DIFF 3 -"
  diff attempt3 solutions/solution3
  echo "- END DIFF -"

  echo "Diff tests complete."

  echo "Wordsearch program outputs available as attempts/attempt[N].txt"
else
	echo "Make failed. start.sh will now stop."
fi
