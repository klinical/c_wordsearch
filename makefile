CC = gcc
CFLAGS := -Wall -ansi -pedantic -g -c
CHKFLAGS := -Wall -Wextra -Wparentheses -ansi -pedantic -pedantic-errors -Wextra -O
TARGET = wordsearch
UTIL = utility

default: $(TARGET)
$(TARGET): $(TARGET).o $(UTIL).o
	$(CC) $(UTIL).o $(TARGET).o -o $(TARGET)

$(UTIL).o: $(UTIL).c includes/$(UTIL).h
	$(CC) -I ./includes $(CFLAGS) $(UTIL).c -o $(UTIL).o

$(TARGET).o: $(TARGET).c includes/$(UTIL).h
	$(CC) -I ./includes $(CFLAGS) $(UTIL).c $(TARGET).c

check: $(TARGET)
	@$(CC) $(CHKFLAGS) $(TARGET).c $(UTIL).c -o /dev/null

clean:
	@$(RM) $(TARGET)
	@$(RM) $(TARGET).o
	@$(RM) $(UTIL).o
