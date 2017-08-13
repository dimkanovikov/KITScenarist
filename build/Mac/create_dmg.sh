#!/bin/bash

#
# сформировать app-файл
#
~/Qt/5.9.1/clang_64/bin/macdeployqt ../Release/bin/scenarist-desktop/Scenarist.app

#
# Скопируем в него системные файлы
#
cp ../../src/bin/scenarist-desktop/info.plist ../Release/bin/scenarist-desktop/Scenarist.app/Contents/
cp ../../src/bin/scenarist-desktop/logo.icns ../Release/bin/scenarist-desktop/Scenarist.app/Contents/Resources/

#
# скопируем app-файл в текущую папку
#
cp -R ../Release/bin/scenarist-desktop/Scenarist.app ./Scenarist.app

#
# подпишем app-файл
#
codesign --deep --force --verify --verbose --sign "Developer ID Application: Dmitry Novikov (9VJUEP4AA5)" Scenarist.app

#
# Создаём русский dmg-файл
#
./make_dmg.sh -i ../../src/bin/scenarist-desktop/logo.icns -b cover.png -c "462:252:176:258" -s "640:400"  Scenarist.app 
mv -f Scenarist.dmg scenarist-setup-$1.dmg
codesign --force --sign "Developer ID Application: Dmitry Novikov (9VJUEP4AA5)" scenarist-setup-$1.dmg

#
# Создаём английский dmg-файл
#
./make_dmg.sh -i ../../src/bin/scenarist-desktop/logo.icns -b cover_en.png -c "462:252:176:258" -s "640:400"  Scenarist.app
mv -f Scenarist.dmg scenarist-setup-$1_en.dmg
codesign --force --sign "Developer ID Application: Dmitry Novikov (9VJUEP4AA5)" scenarist-setup-$1_en.dmg

#
# Создаём испанский dmg-файл
#
./make_dmg.sh -i ../../src/bin/scenarist-desktop/logo.icns -b cover_es.png -c "462:252:176:258" -s "640:400"  Scenarist.app
mv -f Scenarist.dmg scenarist-setup-$1_es.dmg
codesign --force --sign "Developer ID Application: Dmitry Novikov (9VJUEP4AA5)" scenarist-setup-$1_es.dmg

#
# Создаём французский dmg-файл
#
./make_dmg.sh -i ../../src/bin/scenarist-desktop/logo.icns -b cover_fr.png -c "462:252:176:258" -s "640:400"  Scenarist.app
mv -f Scenarist.dmg scenarist-setup-$1_fr.dmg
codesign --force --sign "Developer ID Application: Dmitry Novikov (9VJUEP4AA5)" scenarist-setup-$1_fr.dmg

# удалить app-файл
rm -R ./Scenarist.app
