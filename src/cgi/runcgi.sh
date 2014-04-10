export CONTENT_LENGTH=`wc -b $1`
export CONTENT_TYPE=doesnotmatter
cat  $1 | ./stalinize.cgi
