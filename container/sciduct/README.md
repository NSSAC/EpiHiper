# Sciduct Image

This image contains the sciduct version of epihiper and associated scripts. It depends on the builder image for compiled binaries.  

## Building

docker build --secret id=gh_token,src=/path/to/gh_token -t ghcr.io/nssac/epihiper-sciduct:latest .


