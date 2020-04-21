PROJECT = nave

TARGET = $(PROJECT).elf
OBJS = main.o

CFLAGS= -Ofast -std=gnu11

cdi: romdisk $(TARGET)
	IP_TEMPLATE_FILE=/opt/toolchains/dc/kos/utils/makeip/IP.TMPL $(KOS_BASE)/utils/makeip/makeip ip.txt IP.BIN
	mkisofs -C 0,11702 -V $(PROJECT) -G IP.BIN -r -J -l -o $(PROJECT).iso 1ST_READ.BIN bin/*_romdisk.img
	$(KOS_BASE)/utils/cdi4dc/cdi4dc $(PROJECT).iso $(PROJECT).cdi
	@rm $(PROJECT).iso IP.BIN 1ST_READ.BIN

$(TARGET): $(OBJS)
	$(KOS_CC) $(KOS_CFLAGS) -std=c99 $(KOS_LDFLAGS) -o bin/$(TARGET) $(KOS_START) \
		$(patsubst %, bin/%, $(OBJS)) $(OBJEXTRA) -L$(KOS_BASE)/lib -lGL $(KOS_LIBS) -lm -Wall
	sh-elf-objcopy -R .stack -O binary bin/$(TARGET) bin/output.bin
	$(KOS_BASE)/utils/scramble/scramble bin/output.bin 1ST_READ.BIN

%.o: src/%.c
	kos-cc $(CFLAGS) -c $< -o bin/$@

romdisk:
	-rm -rf romdisk/
	sh make_romdisk.sh

clean:
	-rm -f $(TARGET) $(OBJS)

