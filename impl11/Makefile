# Note: incomplete.
# No header dependency generation/handling yet
# No compilation of shaders

.SUFFIXES:

WARN_FLAGS:=-Wall -Wextra -pedantic -Wno-unused-parameter -Wno-switch -Wno-missing-field-initializers
CXXFLAGS:=-std=c++11 -O2 -DNDEBUG=1 -DMINGW_HAS_SECURE_API=1 -DD3D11_MIN_DEPTH=0.0f -DD3D11_MAX_DEPTH=1.0f
LDFLAGS:=-s -shared -Bsymbolic -static-libgcc -static-libstdc++

all: ddraw.dll

%.rc.o: %.rc
	i686-w64-mingw32-windres -o $@ -i $<

ddraw.dll: ddraw/*.cpp ddraw/ddraw.rc.o ddraw/ddraw.def
	i686-w64-mingw32-g++ $(WARN_FLAGS) $(CXXFLAGS) $(LDFLAGS) -o $@ $^ -lGdiplus -ld3d11 -ldxgi -ldxguid

clean:
	rm -f ddraw.dll ddraw/ddraw.rc.o

.PHONY: all clean
