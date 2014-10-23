#!/bin/bash

# Удалить старый образ
rm ./Scenarist.dmg

# сформировать app-файл
~/Qt/5.3/clang_64/bin/macdeployqt ../Release/bin/Scenarist/Scenarist.app

# скопировать в него ханспел
cp ../Release/libs/hunspell/libhunspell.1.0.0.dylib ../Release/bin/Scenarist/Scenarist.app/Contents/Frameworks/
cp ../Release/libs/hunspell/libhunspell.1.0.dylib ../Release/bin/Scenarist/Scenarist.app/Contents/Frameworks/
cp ../Release/libs/hunspell/libhunspell.1.dylib ../Release/bin/Scenarist/Scenarist.app/Contents/Frameworks/
cp ../Release/libs/hunspell/libhunspell.dylib ../Release/bin/Scenarist/Scenarist.app/Contents/Frameworks/

# скопируем app-файл в текущую папку
cp -R ../Release/bin/Scenarist/Scenarist.app ./Scenarist.app

# запустить скрипт создания dmg-файла
./make_dmg.sh -i ../../src/bin/Scenarist/logo.icns -b cover.png -c "462:252:176:258" -s "640:400"  Scenarist.app
# -c "167:226:425:226" -s "540:400" 

# удалить app-файл
rm -R ./Scenarist.app