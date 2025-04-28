# Builder Image

This image is used to clone the epihiper repo and build binaries.  If you just want to run epihiper on your own, you can use this container.  

## Building

docker build --secret id=gh_token,src=/path/to/gh_token -t ghcr.io/nssac/epihiper-build:latest .



