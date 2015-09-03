#
# TODO: 
# Add keys for architecture i386 or amd64
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
cp -f translations/amd64/ru/control scenarist_amd64/DEBIAN/
fakeroot dpkg-deb --build scenarist_amd64
mv -f scenarist_amd64.deb scenarist-setup-$1_amd64.deb

#
# Make English installer
#
echo Build English version
cp -f translations/amd64/en/control scenarist_amd64/DEBIAN/
fakeroot dpkg-deb --build scenarist_amd64
mv -f scenarist_amd64.deb scenarist-setup-$1_en_amd64.deb

#
# Make Spanish installer
#
echo Build English version
cp -f translations/amd64/es/control scenarist_amd64/DEBIAN/
fakeroot dpkg-deb --build scenarist_i386
mv -f scenarist_amd64.deb scenarist-setup-$1_es_amd64.deb