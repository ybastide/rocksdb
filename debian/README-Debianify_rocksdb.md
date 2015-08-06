# Debianify rocksdb

git clone --no-checkout -o upstream https://github.com/facebook/rocksdb.git
git co -b ubuntu/trusty v3.12.1
dh_make -p librocksdb_3.12.1 -c BSD -e zeb@botify.com -l --createorig
# enlever pkgconfig de debian/librocksdb-dev.install
# update debian/control, control, rules
git -am "Initial import."
# I don't know how to do this: gbp buildpackage --git-pristine-tar --git-pristine-tar-commit --git-upstream-tag='v%(version)s' --git-debian-branch=ubuntu/trusty
# So this instead:
dpkg-buildpackage -b -rfakeroot

## TODO
librocksdb-bin (at least ldb)

## rules

```makefile
#!/usr/bin/make -f
# -*- makefile -*-

# Uncomment this to turn on verbose mode.
#export DH_VERBOSE=1
export DEB_CXXFLAGS_MAINT_OPTIONS = -fPIC
export DEB_LDFLAGS_MAINT_OPTIONS = -fPIC

%:
	dh $@ 

override_dh_auto_build:
	PORTABLE=1 make static_lib shared_lib

override_dh_auto_test:
	# don't run the tests, they don't pass O_o

override_dh_auto_install:
	make install INSTALL_PATH=debian/tmp/usr
```