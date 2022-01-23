#!/bin/bash
# Give 1st argument of the form Hello,World (comma seperated) for Request header values
# Give 2nd argument for verbose ( 1 for verbose 0 for not. 0 is default )
if [ $# -eq 1 ]
then
    _Verbose=0
fi
if [ $# -eq 2 ]
then
_Verbose=$2
fi

function log () {
    if [[ $_Verbose -eq 1 ]]; then
        echo "$@"
    fi
}
export REQ_HEADERS=$1
curl -s -o example.html https://www.example.com/
log "Debug checking"
(echo "IP Address: " && curl http://ip.jsontest.com/) && (echo "Response Headers: " && curl --head http://ip.jsontest.com/)
IFS=',' read -ra req_array <<< "$REQ_HEADERS"
for i in "${req_array[@]}" 
do
    printf "%s: " "$i" && curl -s http://headers.jsontest.com/ | jq -r ."\"$i\""
done
for file in JSONData/*.json
do 
    var=$(curl -s -d "json=$(cat "$file")" -X POST http://validate.jsontest.com/ | jq -r '.validate')
    if [ $var = "true" ]
    then
        echo "${file##*/} : $var" >> valid.txt
    else
        echo "${file##*/} : $var" >> invalid.txt
    fi
done
