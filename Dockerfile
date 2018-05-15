FROM ubuntu:16.04 AS build

RUN apt-get update && apt-get install -y build-essential

COPY . /rtklib/
WORKDIR /rtklib/app/
RUN ./makeall.sh

FROM ubuntu:16.04
COPY --from=build /rtklib/bin /bin
WORKDIR /bin
ENTRYPOINT ["/bin/str2str"]
CMD ["--help"]
