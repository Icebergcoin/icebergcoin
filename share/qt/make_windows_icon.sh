#!/bin/bash
# create multiresolution windows icon
ICON_DST=../../src/qt/res/icons/icebergcoin.ico

convert ../../src/qt/res/icons/icebergcoin-16.png ../../src/qt/res/icons/icebergcoin-32.png ../../src/qt/res/icons/icebergcoin-48.png ${ICON_DST}
