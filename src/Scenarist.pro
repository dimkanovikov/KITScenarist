TEMPLATE = subdirs

SUBDIRS = libs \
    bin/scenarist-desktop.pro
#	bin/scenarist-mobile.pro
#    bin/scenarist-desktop.pro bin/scenarist-mobile.pro

TRANSLATIONS += bin/scenarist-core/Resources/Translations/Scenarist_ru.ts \
    bin/scenarist-core/Resources/Translations/Scenarist_es.ts \
    bin/scenarist-core/Resources/Translations/Scenarist_fr.ts \
    bin/scenarist-core/Resources/Translations/Scenarist_en.ts \
    bin/scenarist-core/Resources/Translations/Scenarist_kz.ts \
    bin/scenarist-core/Resources/Translations/Scenarist_de.ts \
    bin/scenarist-core/Resources/Translations/Scenarist_jp.ts \
    bin/scenarist-core/Resources/Translations/Scenarist_it.ts \
    bin/scenarist-core/Resources/Translations/Scenarist_ua.ts

RESOURCES += \
    bin/scenarist-mobile/scenarist-core/Resources/Resources.qrc
