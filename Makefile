all: linux #windows
release: linux-release #windows-release
debug: linux-debug #windows-debug

linux: linux-debug# linux-release
windows: windows-debug windows-release

windows-debug:
	xmake f -p windows -a x86_64 -m debug
	xmake
	rsync --recursive --progress --delete --update assets/ ./build/windows/x86_64/debug/assets

windows-release:
	xmake f -p windows -a x86_64 -m release
	xmake
	rsync --recursive --progress --delete --update assets/ ./build/windows/x86_64/release/assets

linux-debug:
	xmake f -p linux -a x86_64 -m debug  --debugger=cgdb
	xmake
	rsync --recursive --progress --delete --update assets/ ./build/linux/x86_64/debug/assets/

linux-release:
	xmake f -p linux -a x86_64 -m release
	xmake
	rsync --recursive --progress --delete --update assets/ ./build/linux/x86_64/release/assets/

clean:
	rm -rf build .xmake
