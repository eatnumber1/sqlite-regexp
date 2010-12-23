SOURCES = regexp.c
OBJECTS = $(SOURCES:.c=.o)
EXTENSIONS = regexp.sqlext

CFLAGS += -fPIC -Isqlite3 -Wall
LIBS = -lpcre

.PHONY: all clean

all: regexp.sqlext

clean:
	$(RM) $(OBJECTS) $(EXTENSIONS)

$(EXTENSIONS): %.sqlext: $(OBJECTS)
	$(CC) $(LIBS) -shared $(CFLAGS) $(LDFLAGS) -o $@ $<
