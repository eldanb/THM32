#!/bin/sh
echo Building admin interface
cd wordclock-config-interface
npm run build
cd ..

echo Preparing filesystem image
mkdir data
cp wordclock-config-interface/dist/index.html data/index.html
platformio run --target buildfs --environment prod

echo Building firmware
platformio run --environment prod
