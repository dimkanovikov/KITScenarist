Name: scenarist    
Requires: libc6 >= 2.14, libgcc1 >= 1:3.0, libgl1, libqt5concurrent5, libqt5core5a, libqt5dbus5, libqt5gui5, libqt5multimedia5, libqt5network5, libqt5opengl5, libqt5positioning5, libqt5printsupport5, libqt5qml5, libqt5quick5, libqt5quickwidgets5, libqt5sql5, libqt5svg5, libqt5webchannel5, libqt5webengine5, libqt5webenginecore5, libqt5webenginewidgets5, libqt5widgets5, libqt5xml5, libstdc++6 >= 5.2, zlib1g >= 1:1.1.4
License: GPLv3
Group: Applicaton/Publishing
Summary: KIT Scenarist es una programa de guión, que se desarrolla en Rusia.
%description
KIT Scenarist combina de forma conveniente, un editor de texto y 
un organizador, para proyectos de guión, el editor  ofrece numerosas 
oportunidades para facilitar el trabajo del guionista, incluye 
la posibilidad de sustituir locaciones automaticamente, nombre 
de personajes, corrección ortográfica, agrupar por personajes 
o por escena, exporta directo a PDF y DOCX en todos los idiomas,
temporizador configurable y mas. Las opciones para personajes y locaciones,
permite recolectar todo el material del proyecto en un solo archivo,
y provee un método simple para trabajar.


%files
%defattr(-,root,root,-)
%attr(0755,root,root) /usr/bin/scenarist
/usr/share/applications
/usr/share/KIT
%attr(0644,root,root) /usr/share/pixmaps/scenarist.png
