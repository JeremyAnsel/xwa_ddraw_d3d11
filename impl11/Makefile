# Note: incomplete.
# No header dependency generation/handling yet
# No compilation of shaders
# Not linking .rc file

ddraw.dll: ddraw/*.cpp ddraw/ddraw.def
	i686-w64-mingw32-g++ -std=c++11 -DNDEBUG=1 -DMINGW_HAS_SECURE_API=1 -DD3D11_MIN_DEPTH=0.0f -DD3D11_MAX_DEPTH=1.0f -shared -static-libgcc -static-libstdc++ -O2 -o $@ $^ -lGdiplus -ld3d11 -ldxgi -ldxguid -lddraw
