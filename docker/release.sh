#
#    This file is part of the RealtimeSanitizer (RTSan) project, under the
#    Apache v2.0 license.
#
#    https://github.com/realtime-sanitizer/rtsan
#
set -euxo pipefail
repo=realtimesanitizer/rtsan-clang
amd64_tag=${repo}:manifest-amd64
arm64_tag=${repo}:manifest-arm64
multi_tag=${repo}:latest

docker build --platform linux/amd64 -t ${amd64_tag} -f docker/Dockerfile .
docker build --platform linux/arm64 -t ${arm64_tag} -f docker/Dockerfile .
docker push ${arm64_tag}
docker push ${amd64_tag}

docker manifest rm ${multi_tag}
docker manifest create ${multi_tag} --amend ${arm64_tag} --amend ${amd64_tag}
docker manifest push ${multi_tag}
