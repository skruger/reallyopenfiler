#!/bin/sh

openssl genrsa -out ssl.key/openfiler-dummy-ca-1.key 1024
openssl req -sha1 -new -key ssl.key/openfiler-dummy-ca-1.key -x509 -days 3650 -out ssl.crt/openfiler-dummy-ca-1.crt

openssl genrsa -out ssl.key/openfiler-dummy-server-1.key 1024
openssl req -sha1 -new -key ssl.key/openfiler-dummy-server-1.key -out ssl.csr/openfiler-dummy-server-1.csr
openssl x509 -sha1 -req -days 3650 -in ssl.csr/openfiler-dummy-server-1.csr -CA ssl.crt/openfiler-dummy-ca-1.crt -CAkey ssl.key/openfiler-dummy-ca-1.key -out ssl.crt/openfiler-dummy-server-1.crt

