#!/bin/bash

#
# Удалить старый образ
#
rm ./Scenarist.dmg

#
# сформировать app-файл
#
~/Qt/5.5/clang_64/bin/macdeployqt ../Release/bin/Scenarist/Scenarist.app

#
# скопировать в него библиотеки
#
# hunspell
#
cp ../Release/libs/hunspell/libhunspell.1.dylib ../Release/bin/Scenarist/Scenarist.app/Contents/Frameworks/
#
# fileformats
#
cp -H ../Release/libs/fileformats/libfileformats.1.dylib ../Release/bin/Scenarist/Scenarist.app/Contents/Frameworks/
#
# webloader
#
cp -H ../Release/libs/webloader/libwebloader.1.dylib ../Release/bin/Scenarist/Scenarist.app/Contents/Frameworks/

#
# настроим пути поиска библиотек
#
# для fileformats
#
install_name_tool -change /Users/macuser/Qt/5.5/clang_64/lib/QtCore.framework/Versions/5/QtCore @executable_path/../Frameworks/QtCore.framework/Versions/5/QtCore ../Release/bin/Scenarist/Scenarist.app/Contents/Frameworks/libfileformats.1.dylib 
install_name_tool -change /Users/macuser/Qt/5.5/clang_64/lib/QtGui.framework/Versions/5/QtGui @executable_path/../Frameworks/QtGui.framework/Versions/5/QtGui ../Release/bin/Scenarist/Scenarist.app/Contents/Frameworks/libfileformats.1.dylib 
#
# для webloader
#
install_name_tool -change /Users/macuser/Qt/5.5/clang_64/lib/QtCore.framework/Versions/5/QtCore @executable_path/../Frameworks/QtCore.framework/Versions/5/QtCore ../Release/bin/Scenarist/Scenarist.app/Contents/Frameworks/libwebloader.1.dylib 
install_name_tool -change /Users/macuser/Qt/5.5/clang_64/lib/QtNetwork.framework/Versions/5/QtNetwork @executable_path/../Frameworks/QtNetwork.framework/Versions/5/QtNetwork ../Release/bin/Scenarist/Scenarist.app/Contents/Frameworks/libwebloader.1.dylib 
install_name_tool -change /Users/macuser/Qt/5.5/clang_64/lib/QtXml.framework/Versions/5/QtXml @executable_path/../Frameworks/QtXml.framework/Versions/5/QtXml ../Release/bin/Scenarist/Scenarist.app/Contents/Frameworks/libwebloader.1.dylib 

#
# Скопируем в него системные файлы
#
cp ../../src/bin/Scenarist/info.plist ../Release/bin/Scenarist/Scenarist.app/Contents/
cp ../../src/bin/Scenarist/logo.icns ../Release/bin/Scenarist/Scenarist.app/Contents/Resources/

#
# скопируем app-файл в текущую папку
#
cp -R ../Release/bin/Scenarist/Scenarist.app ./Scenarist.app

# запустить скрипт создания dmg-файла
./make_dmg.sh -i ../../src/bin/Scenarist/logo.icns -b cover.png -c "462:252:176:258" -s "640:400"  Scenarist.app 

# удалить app-файл
rm -R ./Scenarist.app