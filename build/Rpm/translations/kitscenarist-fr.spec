Name: scenarist
Requires: libc6 >= 2.14, libgcc1 >= 1:3.0, libgl1, libqt5concurrent5, libqt5core5a, libqt5dbus5, libqt5gui5, libqt5multimedia5, libqt5network5, libqt5opengl5, libqt5positioning5, libqt5printsupport5, libqt5qml5, libqt5quick5, libqt5quickwidgets5, libqt5sql5, libqt5svg5, libqt5webchannel5, libqt5webengine5, libqt5webenginecore5, libqt5webenginewidgets5, libqt5widgets5, libqt5xml5, libstdc++6 >= 5.2, zlib1g >= 1:1.1.4
License: GPLv3
Group: Applicaton/Publishing
Summary: KIT Scenarist est un programme de scénario qui se développe en Russie.
%description
KIT Scenarist combine idéalement un éditeur de texte et un organisateur pour 
les projets de scénario, l'éditeur offre de nombreuses possibilités pour 
faciliter le travail de l'écrivain, comprend la capacité de remplacer 
automatiquement les emplacements, les noms des personnages, la vérification 
orthographique, le groupe par caractère ou de la scène, les exportations 
vers PDF et DOCX vivent dans toutes les langues, minuterie configurable et 
plus encore. Les options pour les personnages et les lieux, peuvent recueillir
tout le matériel dans un fichier de projet, et fournit une méthode simple pour
travailler.

%files
%defattr(-,root,root,-)
%attr(0755,root,root) /usr/bin/scenarist
/usr/share/applications
/usr/share/KIT
%attr(0644,root,root) /usr/share/pixmaps/scenarist.png
