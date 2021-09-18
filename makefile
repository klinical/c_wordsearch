CC = gcc
CFLAGS := -Wall -ansi -pedantic -c -g
CHKFLAGS := -Wall -Wextra -Wparentheses -ansi -pedantic -pedantic-errors -Wextra -O -c
TARGET = wordsearch

default: $(TARGET)

$(TARGET).o: $(TARGET).c
	$(CC) $(CFLAGS) $(TARGET).c -o $(TARGET).o

$(TARGET): $(TARGET).o
	$(CC) $(TARGET).o -o $(TARGET)

check: $(TARGET)
	@$(CC) $(CHKFLAGS) $(TARGET).c -o /dev/null

clean:
	@$(RM) $(TARGET)
	@$(RM) $(TARGET).o
