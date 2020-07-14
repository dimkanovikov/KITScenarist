Name: scenarist
Requires: libc6 >= 2.14, libgcc1 >= 1:3.0, libgl1, libqt5concurrent5, libqt5core5a, libqt5dbus5, libqt5gui5, libqt5multimedia5, libqt5network5, libqt5opengl5, libqt5positioning5, libqt5printsupport5, libqt5qml5, libqt5quick5, libqt5quickwidgets5, libqt5sql5, libqt5svg5, libqt5webchannel5, libqt5webengine5, libqt5webenginecore5, libqt5webenginewidgets5, libqt5widgets5, libqt5xml5, libstdc++6 >= 5.2, zlib1g >= 1:1.1.4
License: GPLv3
Group: Applicaton/Publishing
Summary: КИТ Сценарист - это программа для написания сценариев, которая разрабатывается в России и ориентирована на российские стандарты в области кинопроизводства.
%description
КИТ Сценарист сочетает в себе удобный текстовый редактор сценария и органайзер
проекта. Текстовый редактор реализует многочисленные возможности,
способствующие работе сценариста, среди которых: возможность автоподстановки
локаций, имён персонажей; проверка правописания на ходу; поддержка
группирования сцен, групп сцен; безболезненный экспорт в форматы PDF и RTF;
настраиваемый подсчёт хронометража и многое другое. Функции для работы 
с персонажами и локациями позволяют собрать весь материал по проекту 
в одном месте, и предоставляют простой способ для работы с ним.

%files
%defattr(-,root,root,-)
%attr(0755,root,root) /usr/bin/scenarist
/usr/share/applications
/usr/share/KIT
%attr(0644,root,root) /usr/share/pixmaps/scenarist.png
