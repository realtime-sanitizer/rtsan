# RealtimeSanitizer Docker images

The RTSan team occasionally releases a [Docker image](https://hub.docker.com/r/realtimesanitizer/rtsan-clang) with a recent version of LLVM (including RTSan) installed. This is designed purely to make it easy to try RealtimeSanitizer in the time period before it appears in downstream installations like Apple Clang. There are no guarantees about stability or backwards compatibility here - but please let us know if you have any difficulties and we will do our best to help.

The remainder of this document is intended for RTSan developers only.

## Developers

### How to release a new version of the realtimesanitizer/rtsan image

1. Check out the version of llvm-project you wish to build for, and
2. Point our `docker/release.sh` script at it:

```sh
./docker/release.sh path/to/the/llvm-project/repo
```
