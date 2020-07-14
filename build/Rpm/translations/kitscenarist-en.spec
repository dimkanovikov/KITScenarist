Name: scenarist
Requires: libc6 >= 2.14, libgcc1 >= 1:3.0, libgl1, libqt5concurrent5, libqt5core5a, libqt5dbus5, libqt5gui5, libqt5multimedia5, libqt5network5, libqt5opengl5, libqt5positioning5, libqt5printsupport5, libqt5qml5, libqt5quick5, libqt5quickwidgets5, libqt5sql5, libqt5svg5, libqt5webchannel5, libqt5webengine5, libqt5webenginecore5, libqt5webenginewidgets5, libqt5widgets5, libqt5xml5, libstdc++6 >= 5.2, zlib1g >= 1:1.1.4
License: GPLv3
Group: Applicaton/Publishing
Summary: KIT Scenarist is screenwriting software, which developed in Russia.
%description
KIT Scenarist combines convenient script editor and organizer of the
screenplay project. Script editor implements numerous opportunities
to facilitate the work of the screenwriter, including: the ability
to autosubstitution locations, names of characters; check the spelling
on the fly; support the grouping of scenes and groups self; painless
exporting to PDF and DOCX in all world languages; customizable timing
counting and more. Options for the characters and locations allow 
to collect all the material on the project in one file,
and provide a simple way to work with it.

%files
%defattr(-,root,root,-)
%attr(0755,root,root) /usr/bin/scenarist
/usr/share/applications
/usr/share/KIT
%attr(0644,root,root) /usr/share/pixmaps/scenarist.png
