BOARD = adafruit:samd:adafruit_feather_m0
PORT = /dev/ttyACM0

compile:
	arduino-cli compile -b ${BOARD}

upload:
	arduino-cli upload -b ${BOARD} -p ${PORT}
