bool mountRomdisk(char *filename, char *mountpoint) {
    void *buffer;
    ssize_t size = fs_load(filename, &buffer);

    if (size != -1) {
        fs_romdisk_mount(mountpoint, buffer, 1);
        return true;
    }
    else {
        return false;
    }
}

void umountRomdisk(char *mountpoint) {
    fs_romdisk_unmount(mountpoint);
}
