cp -f ./vfs/link.c /usr/src/minix/servers/vfs/link.c;
cp -f ./vfs/open.c /usr/src/minix/servers/vfs/open.c;
cp -f ./vfs/read.c /usr/src/minix/servers/vfs/read.c;
cp -f ./mfs/read.c /usr/src/minix/fs/mfs/read.c;
cp -f ./mfs/write.c /usr/src/minix/fs/mfs/write.c;
cp -f ./const.h /usr/src/minix/include/minix/const.h;
cp -f ./stat.h /usr/src/sys/sys/stat.h;
cd /usr/src && make build MKUPDATE=yes