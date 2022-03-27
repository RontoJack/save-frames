CC=gcc

save-frame: save-frame.c
	$(CC) -o save-frame save-frame.c `pkg-config --cflags --libs gstreamer-1.0`
