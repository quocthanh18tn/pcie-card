APP = librru
 
LIBSOURCES= json/json.c \
			json/jsmn.c \
			axicore/axicore.c \
			config/rru_config.c \
			osinfo/osinfo.c \
			rf/rf.c \
			qsfp/qsfp.c \
			librru.c \
			rrulog.c \
			rrulocal.c \
			smbus.c \
			hwinfo/hwinfo.c \
			ams/ams.c \
			selftest/selftest.c \
			reg/reg.c \
			pa/pa.c \
			calib/calib.c \
			gpio_led/gpio_led.c \
			gpio_led/gpiolib.c \
			clock/clock.c 
OUTS = *.o
NAME := rru
MAJOR = 1.0
MINOR = 1
VERSION = $(MAJOR).$(MINOR)
 
all: lib$(NAME).so
lib$(NAME).so.$(VERSION): $(OUTS)
	$(CC) $(LDFLAGS) $(OUTS) -shared -Wl,-soname,lib$(NAME).so.$(MAJOR) -o lib$(NAME).so.$(VERSION) -liio -lc
lib$(NAME).so: lib$(NAME).so.$(VERSION)
	rm -f lib$(NAME).so.$(MAJOR) lib$(NAME).so
	ln -s lib$(NAME).so.$(VERSION) lib$(NAME).so.$(MAJOR)
	ln -s lib$(NAME).so.$(MAJOR) lib$(NAME).so
 
%.o: %.c
	$(CC) $(CFLAGS) -c -fPIC $(LIBSOURCES) -isystem ./.
 
clean:
	rm -rf *.o *.so *.so.*
