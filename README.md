# SANTI_UIO

## Coding Style
Use clang-format to fix the coding style
```
cd santi_uio
clang-format -i src/*.c
```

## tools

### uioctl
uioctl is used to test UIO device(/dev/uioX) in filesystem
Put uioctl under openwrt/pacakge/ and enable it in menuconfig
```
cp tools/uioctl openwrt/package
make menuconfig //select uioctl under Development catagory
make package/uioctl/compile V=s
```

