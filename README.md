# xavine
Kleine Game-Engine im Eigenbau.

## dependencies:
### xmake
[Hier](https://xmake.io/#/guide/installation) sind Installationsinstruktionen. Alternativ kann man auch von [GitHub](https://github.com/xmake-io/xmake/releases) die aktuelle version herunterladen.
### irgendein Compiler für die aktuelle Plattform
Unter Windows wird msvc2022 nicht unterstützt. mcsv2019 kann man [hier](https://docs.microsoft.com/en-us/visualstudio/releases/2019/history) herunterladen.
### funktionierende Internetverbindung
Ich werds nicht für dich debuggen 😜.

## compilierung:
Im xavine-Verzeichnis ein Terminal aufmachen und dann folgende Befehle ausführen:

compilen:
```
xmake
```

spiel starten:
```
xmake run
```

## overlays:
Mesa Vulkan Overlay
```
VK_INSTANCE_LAYERS=VK_LAYER_MESA_overlay VK_LAYER_MESA_OVERLAY_CONFIG=position=top-right xmake r
```

MangoHud
```
mangohud xmake r
```

TEST