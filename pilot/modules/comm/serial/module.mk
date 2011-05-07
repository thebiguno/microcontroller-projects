SOURCES += modules/comm/serial/communication_serial.c lib/serial/serial.c lib/serial/serial_async_rx.c lib/serial/serial_sync_tx.c lib/ring/ring.c
CDEFS += -DSERIAL_BUFFER_SIZE=255
