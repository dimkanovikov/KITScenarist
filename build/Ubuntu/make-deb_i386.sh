#
# TODO: 
# Add keys for architecture i386 or i386
# Add permissions check for control file: need 755
# Add version update availability
# Rename result file to choosing version and architecture
#

#
# $1 - application version
#

#
# Make Russian installer
#
echo Build Russian version
cp -f translations/i386/ru/control scenarist_i386/DEBIAN/
fakeroot dpkg-deb --build scenarist_i386
mv -f scenarist_i386.deb scenarist-setup-$1_i386.deb

#
# Make English installer
#
echo Build English version
cp -f translations/i386/en/control scenarist_i386/DEBIAN/
fakeroot dpkg-deb --build scenarist_i386
mv -f scenarist_i386.deb scenarist-setup-$1_en_i386.deb

#
# Make Spanish installer
#
echo Build Spanish version
cp -f translations/i386/es/control scenarist_i386/DEBIAN/
fakeroot dpkg-deb --build scenarist_i386
mv -f scenarist_i386.deb scenarist-setup-$1_es_i386.deb

#
# Make French installer
#
echo Build French version
cp -f translations/i386/fr/control scenarist_i386/DEBIAN/
fakeroot dpkg-deb --build scenarist_i386
mv -f scenarist_i386.deb scenarist-setup-$1_fr_i386.deb