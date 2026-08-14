{pkgs}: {
  deps = [
    pkgs.grub2
    pkgs.xorriso
    pkgs.nasm
    pkgs.wineWowPackages.stable
    pkgs.qemu
  ];
}
