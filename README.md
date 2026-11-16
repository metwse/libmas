# libmas

Collection of minimal assemblers.


## Building

Currently, only ARM Thum-2 platform back end is implemented. You can directly
build this project using `make -j` if you are on an ARM computer. To
cross-compile from another CPU architecture, you can install GNU ARM
cross-compiler:

                # vvv your operating system's ARM cross-compiler
    export CC=arm-linux-gnueabi-gcc
    export AR=arm-linux-gnueabi-ar
    make -j

## `contribute -Wai-slop`
<img width="96" height="96" alt="no-ai-slop" align="right" src="https://github.com/user-attachments/assets/bca16d5a-a6fe-4cbf-b41f-1176e000cff2" />

Contributions are welcome! Please check our
[Code of Conduct](http://github.com/metwse/code-of-conduct) before submitting
a patch.
