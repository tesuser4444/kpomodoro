# kpomodoro

Temporizador Pomodoro nativo para KDE Plasma (C++ / Qt 6 / Kirigami / KDE Frameworks 6).

- Fases de enfoque, descanso corto y descanso largo configurables
- Notificaciones de Plasma al terminar cada fase
- Icono en la bandeja del sistema con el tiempo restante

## Dependencias (Arch / CachyOS)

    sudo pacman -S --needed base-devel cmake ninja extra-cmake-modules \
        qt6-base qt6-declarative kirigami qqc2-desktop-style knotifications kstatusnotifieritem

## Compilar e instalar

    cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=$HOME/.local
    cmake --build build
    cmake --install build

Para instalar en todo el sistema usa `-DCMAKE_INSTALL_PREFIX=/usr` y `sudo cmake --install build`.

## Licencia

GPL-3.0-or-later. Consulta el archivo `LICENSE`.

## Versionado y releases

Se sigue [SemVer](https://semver.org/lang/es/). La versión vive en `CMakeLists.txt`.

    scripts/release.sh 1.1.0
    git push origin main v1.1.0

El tag `v*` dispara el workflow *Release*, que compila y publica un tarball
(`.tar.gz`), un paquete Arch (`.pkg.tar.zst`) y `SHA256SUMS` en GitHub Releases.
Un tag con sufijo (`v1.2.0-rc.1`) se publica como pre-release. El workflow *CI*
compila cada push/PR y sube el binario como artifact.
