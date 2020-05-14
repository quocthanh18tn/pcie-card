APP = i2c-test spi-test
SCRIPT = gpio-test selftest

OBJS = $(addsuffix .o,$(APP))

all: build

build: $(APP) $(SCRIPT)

clean:

$(APP): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $@.o $(LDLIBS)

$(SCRIPT):
	# Check if $(SCRIPT) is changed
