TEMPLATE = subdirs

SUBDIRS = libs bin/scenarist-desktop.pro bin/scenarist-mobile.pro

TRANSLATIONS += bin/Scenarist/Resources/Translations/Scenarist_ru.ts \
	bin/Scenarist/Resources/Translations/Scenarist_es.ts \
	bin/Scenarist/Resources/Translations/Scenarist_fr.ts \
	bin/Scenarist/Resources/Translations/Scenarist_en.ts

RESOURCES += \
    bin/scenarist-mobile/Resources/Resources.qrc
