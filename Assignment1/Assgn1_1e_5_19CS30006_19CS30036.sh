#!/bin/bash
# Give 1 argument of the form Hello,World (comma seperated) for Request header values
export REQ_HEADERS=$1
curl -s -o example.html https://www.example.com/
(echo "IP Address: " && curl http://ip.jsontest.com/) && (echo "Response Headers: " && curl --head http://ip.jsontest.com/)
IFS=',' read -ra req_array <<< "$REQ_HEADERS"
for i in "${req_array[@]}" 
do
    printf "%s: " "$i" && curl -s http://headers.jsontest.com/ | jq -r ."\"$i\""
done
