# OpenGL (c)2013-2014 Josh PH3NOM Pearson

TARGET = main.elf
OBJS = main.o
OBJS += romdisk.o

all: rm-elf gfx $(TARGET)

main.o: src/main.c
	kos-cc $(CFLAGS) -std=c99 -c $< -o bin/$@ -Wall

# include $(KOS_BASE)/Makefile.rules

gfx: spaceship.vq
	mv assets/spaceship.vq romdisk/spaceship.vq

clean:
	-rm -f $(TARGET) $(OBJS)

rm-elf:
	-rm -f $(TARGET) 

$(TARGET): $(OBJS) 
	$(KOS_CC) $(KOS_CFLAGS) -std=c99 $(KOS_LDFLAGS) -o bin/$(TARGET) $(KOS_START) \
		$(patsubst %, bin/%, $(OBJS)) $(OBJEXTRA) -L$(KOS_BASE)/lib -lGL $(KOS_LIBS) -lm -Wall
	sh-elf-objcopy -R .stack -O binary bin/$(TARGET) bin/output.bin
	$(KOS_BASE)/utils/scramble/scramble bin/output.bin 1ST_READ.BIN

spaceship.vq: assets/spaceship.png
	$(KOS_BASE)/utils/vqenc/vqenc -t -v assets/spaceship.png

romdisk.img: 
	$(KOS_GENROMFS) -f bin/romdisk.img -d romdisk -v

romdisk.o: romdisk.img
	$(KOS_BASE)/utils/bin2o/bin2o bin/romdisk.img romdisk bin/romdisk.o

run: $(TARGET)
	$(KOS_LOADER) $(TARGET)

dist:
	rm -f $(OBJS) 
	$(KOS_STRIP) $(TARGET)

