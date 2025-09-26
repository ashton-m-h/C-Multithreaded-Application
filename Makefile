CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=gnu99 -pthread
DEBUG = -g

CASCLIBDIR = /usr/lib64
CASCLFLAGS = -L$(CASCLIBDIR) -lopencv_core -lopencv_imgcodecs -lopencv_objdetect -lopencv_imgproc
TARGETS = uqfaceclient uqfacedetect

.DEFAULT_GOAL := all
all: $(TARGETS)

debug: CFLAGS += $(DEBUG)
debug: clean $(TARGETS)

.PHONY: all debug clean

uqfaceclient: main_faceclient.c parse_cmdline_client.o clnt_errs.o check_params_client.o clnt_cleaner.o open_port.o make_clnt_request.o clnt_receive.o rw_streams.o
	$(CC) $(CFLAGS) $^ -o $@

open_port.o: open_port.c
	$(CC) $(CFLAGS) -c $^ -o $@

parse_cmdline_client.o: parse_cmdline_client.c
	$(CC) $(CFLAGS) -c $^ -o $@

clnt_errs.o: clnt_errs.c
	$(CC) $(CFLAGS) -c $^ -o $@

check_params_client.o: check_params_client.c
	$(CC) $(CFLAGS) -c $^ -o $@

clnt_cleaner.o: clnt_cleaner.c
	$(CC) $(CFLAGS) -c $^ -o $@

make_clnt_request.o: make_clnt_request.c
	$(CC) $(CFLAGS) -c $^ -o $@

clnt_receive.o: clnt_receive.c
	$(CC) $(CFLAGS) -c $^ -o $@

rw_streams.o: rw_streams.c
	$(CC) $(CFLAGS) -c $^ -o $@

uqfacedetect: main_faceserv.c parse_cmdline_serv.o serv_errs.o serv_cleaner.o listen_port.o check_params_serv.o load_cascades.o process_connects.o rw_streams.o serv_receive.o check_serv_rec.o process_image.o serv_sender.o send_badrequest.o sighup_handler.o
	$(CC) $(CFLAGS) $^ $(CASCLFLAGS) -o $@

parse_cmdline_serv.o: parse_cmdline_serv.c
	$(CC) $(CFLAGS) -c $^ -o $@

serv_errs.o: serv_errs.c
	$(CC) $(CFLAGS) -c $^ -o $@

serv_cleaner.o: serv_cleaner.c
	$(CC) $(CFLAGS) -c $^ -o $@

listen_port.o: listen_port.c
	$(CC) $(CFLAGS) -c $^ -o $@

check_params_serv.o: check_params_serv.c
	$(CC) $(CFLAGS) -c $^ -o $@

load_cascades.o: load_cascades.c
	$(CC) $(CFLAGS) -c $^ -o $@

process_connects.o: process_connects.c
	$(CC) $(CFLAGS) -c $^ -o $@

serv_receive.o: serv_receive.c
	$(CC) $(CFLAGS) -c $^ -o $@

check_serv_rec.o: check_serv_rec.c
	$(CC) $(CFLAGS) -c $^ -o $@

process_image.o: process_image.c
	$(CC) $(CFLAGS) -c $^ -o $@

serv_sender.o: serv_sender.c
	$(CC) $(CFLAGS) -c $^ -o $@

send_badrequest.o: send_badrequest.c
	$(CC) $(CFLAGS) -c $^ -o $@

sighup_handler.o: sighup_handler.c
	$(CC) $(CFLAGS) -c $^ -o $@

clean:
	rm -rf $(TARGETS) *.o
