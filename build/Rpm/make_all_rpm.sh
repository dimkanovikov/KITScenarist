#!/bin/sh

#$1 - version
#$2 - release version

#TODO: parallelize loops

supported_langs=`ls translations | grep '.spec' | sed 's/kitscenarist-\(.*\).spec/\1/g'`

for platform in "x86_64" "i386"; do
	for lang in $supported_langs; do
		echo "Build $lang $platform installer"
	
		sed "/License: GPLv3/a Version: $1" translations/kitscenarist-$lang.spec | sed "/License: GPLv3/a Release: $2" > kitscenarist-$1-$2.spec
	
		rm scenarist_$platform/usr/share/applications/*
	
		cp "translations/scenarist-$lang.desktop" "scenarist_$platform/usr/share/applications/"
	
		rpmbuild --buildroot=$(pwd)/scenarist_$platform --target=$platform --noclean -bb kitscenarist-$1-$2.spec > /dev/null

		if [ $? -ne 0 ]; then
		    echo "Something went wrong"
		    echo "Return code was not zero but $retval"
		    continue
		fi

		mv ~/rpmbuild/RPMS/$platform/scenarist-$1-$2.$platform.rpm scenarist-$1-$2.$platform-$lang.rpm

		echo "Success"
	
	done

done

rm -r ~/rpmbuild
rm *.spec
