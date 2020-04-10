mkdir -p romdisk/

asset_folder=$(find assets/)
first=1

for file in $asset_folder; do
    if [ $first == 1 ]; then
        first=0
        continue
    fi

    romdisk_file=$(echo "$file" | sed "s/^assets/romdisk/")
    if [ -d $file ]; then
        echo "Directory $file -> $romdisk_file"
        mkdir -p $romdisk_file
    else
        echo "File $file -> $romdisk_file"
        if [ ${file: -4} == ".png" ]; then
            config=$(echo "$file" | sed "s/\.png/\.enc/")
            $KOS_BASE/utils/vqenc/vqenc `cat $config` $file
            file=$(echo "$file" | sed "s/\.png/\.vq/")
            romdisk_file=$(echo "$romdisk_file" | sed "s/\.png/\.vq/")
            mv $file $romdisk_file
        elif [ ${file: -4} == ".enc" ]; then
            true
        else
            cp $file $romdisk_file
        fi
    fi
done
