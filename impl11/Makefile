# Note: incomplete.
# No header dependency generation/handling yet
# No compilation of shaders

VERSION:=$(shell grep FileVersion ddraw/ddraw.rc | sed -e 's/.*, "//' -e 's/"//')
DLLVERSION=$(shell pev -p Release/ddraw.dll)

.SUFFIXES:

WARN_FLAGS:=-Wall -Wextra -pedantic -Wno-unused-parameter -Wno-switch -Wno-missing-field-initializers
CXXFLAGS:=-msse -msse2 -std=c++11 -pipe -O2 -DNDEBUG=1 -DMINGW_HAS_SECURE_API=1 -DD3D11_MIN_DEPTH=0.0f -DD3D11_MAX_DEPTH=1.0f -DD3DCOLORVALUE_DEFINED
LDFLAGS:=-s -shared -Bsymbolic -static-libgcc -static-libstdc++

all: ddraw.dll

%.rc.o: %.rc
	i686-w64-mingw32-windres -o $@ -i $<

ddraw.dll: ddraw/*.cpp ddraw/ddraw.rc.o ddraw/ddraw.def
	i686-w64-mingw32-g++ $(WARN_FLAGS) $(CXXFLAGS) $(LDFLAGS) -o $@ $^ -lgdiplus -ld3d11 -ldxgi -ldxguid

clean:
	rm -f ddraw.dll ddraw/ddraw.rc.o xwing_ddraw_d3d11_*.zip


# ./ prefix to make 7z store the files without path
xwing_ddraw_d3d11_$(VERSION).zip: ddraw/ddraw.cfg Release/ddraw.dll ../LICENSE.txt ../readme.txt
	rm -f $@
	unix2dos -k ddraw/ddraw.cfg ../LICENSE.txt ../readme.txt
	7z a -mx9 $@ $(addprefix ./,$^)

ifeq ("$(VERSION).0","$(DLLVERSION)")
release: xwing_ddraw_d3d11_$(VERSION).zip
	gpg -ab $^
	git tag -s v$(VERSION)
else
release:
	@echo Version mismatch: Repo $(VERSION), DLL $(DLLVERSION)
	@false
endif

.PHONY: all clean release
