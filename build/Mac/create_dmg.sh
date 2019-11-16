#!/bin/bash

#
# сформировать app-файл
#
~/Qt/5.9.8/clang_64/bin/macdeployqt ../Release/bin/scenarist-desktop/Scenarist.app

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
codesign --force --deep --sign "Developer ID Application: Dmitry Novikov (9VJUEP4AA5)" --options "runtime" "Scenarist.app/"

#
# сформировать архив с приложением
#
zip -r Scenarist.zip Scenarist.app

#
# отправить архив на проверку
#
xcrun altool --notarize-app -t osx -f Scenarist.zip --primary-bundle-id="com.dimkanovikov.kit.scenarist.macosx" -u "novikovdimka@gmail.com"

#
# подождать 3 минуты
#
sleep 3m

#
# проверить статус нотаризации (тут нужно указать юид возвращённый предыдущей командой)
#
# xcrun altool --notarization-info UUID -u novikovdimka@gmail.com

#
# удалить архив
#
rm -R Scenarist.zip

#
# поместить тикет в приложение
#
xcrun stapler staple Scenarist.app

#
# Создаём русский dmg-файл
#
./make_dmg.sh -i ../../src/bin/scenarist-desktop/logo.icns -b cover.png -c "462:252:176:258" -s "640:400"  Scenarist.app
mv -f Scenarist.dmg scenarist-setup-$1.dmg

#
# Создаём английский dmg-файл
#
./make_dmg.sh -i ../../src/bin/scenarist-desktop/logo.icns -b cover_en.png -c "462:252:176:258" -s "640:400"  Scenarist.app
mv -f Scenarist.dmg scenarist-setup-$1_en.dmg

#
# Создаём испанский dmg-файл
#
./make_dmg.sh -i ../../src/bin/scenarist-desktop/logo.icns -b cover_es.png -c "462:252:176:258" -s "640:400"  Scenarist.app
mv -f Scenarist.dmg scenarist-setup-$1_es.dmg

#
# Создаём французский dmg-файл
#
./make_dmg.sh -i ../../src/bin/scenarist-desktop/logo.icns -b cover_fr.png -c "462:252:176:258" -s "640:400"  Scenarist.app
mv -f Scenarist.dmg scenarist-setup-$1_fr.dmg

#
# удалить app-файл
#
rm -R ./Scenarist.app
